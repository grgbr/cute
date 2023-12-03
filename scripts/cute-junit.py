#!/usr/bin/env python3
# -*- coding: utf-8 -*-
################################################################################
# SPDX-License-Identifier: GPL-3.0-only
#
# This file is part of CUTe.
# Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

from __future__ import annotations
from os.path import basename, dirname
from sys import argv, exit, stderr

arg0 = basename(argv[0])

try:
    from typing import Iterator, Literal
    from argparse import ArgumentParser
    from tempfile import mkstemp
    from os import replace, sync, close, unlink
    from enum import Enum, auto
    from math import modf
    from datetime import date, timedelta, datetime
    from junitparser import JUnitXml, \
                            TestSuite as JUnitTestSuite, \
                            TestCase as JUnitTestCase, \
                            Failure as JUnitFailure, \
                            Error as JUnitError, \
                            Skipped as JUnitSkipped, \
                            Element as JUnitElement, \
                            Properties as JUnitProperties
    from rich.style import Style
    from rich.text import Text
    from rich.table import Table, Column
    from rich.columns import Columns
    from rich.padding import Padding, PaddingDimensions
    from rich.console import Console, ConsoleOptions, RenderResult, \
                             RenderableType, ConsoleRenderable
except Exception as e:
    print("{}: Python module import failed: {}.".format(arg0, e), file=stderr)
    exit(1)

class CuteStatus(Enum):
    PASS = auto()
    SKIP = auto()
    FAIL = auto()
    EXCP = auto()
    OFF = auto()
    NONE = auto()

    def __str__(self) -> str:
        return self.name.lower()


class CuteBase:
    def __init__(self,
                 junit: JUnitElement,
                 parent: CuteSuite | None,
                 status: CuteStatus,
                 package: str | None,
                 version: str | None,
                 hostname: str | None) -> None:
        if not junit.name or len(junit.name) == 0:
            raise Exception("cannot initialize JUnit base element: "
                            "invalid name: '{}'".format(junit.name))
        self._name = junit.name
        self._status = status
        self._elapsed = None
        if self._status != CuteStatus.OFF:
            if junit.time >= 0:
                secs, usecs = modf(junit.time)
                self._elapsed = timedelta(seconds = secs, microseconds = usecs)
        attrs = junit._elem.attrib
        self._timestamp = None
        if self._status != CuteStatus.OFF:
            if 'timestamp' in attrs.keys():
                try:
                    self._timestamp = datetime.fromisoformat(attrs['timestamp'])
                except Exception:
                    pass
        self._package = None
        self._version = None
        if package and len(package) > 0:
            self._package = package
            if version and len(version) > 0:
                self._version = version
        self._hostname = None
        if hostname and len(hostname) > 0 and status != CuteStatus.OFF:
            self._hostname = hostname
        self._path = None
        self._line = None
        if 'file' in attrs.keys():
            path = attrs['file']
            if path and len(path) > 0:
                self._path = path
            if 'line' in attrs.keys():
                line = attrs['line']
                if line and len(line) > 0:
                    self._line = int(line)
        self._parent = parent
        if parent:
            self._depth = parent.depth + 1
            parent.register(self)
        else:
            self._depth = 0
        self._count: int | None = None
        self._exec: int | None = None
        self._off: int | None = None
        self._pass: int | None = None
        self._skip: int | None = None
        self._fail: int | None = None
        self._excp: int | None = None
    
    def __repr__(self) -> str:
        return str(self.__dict__)

    @property
    def expose(self) -> dict[str, object]:
        return {
                'name':         self.name,
                'full_name':    self.full_name,
                'type':         self.kind,
                'has_pkg':      self.package is not None,
                'pkg':          self.package,
                'has_vers':     self.version is not None,
                'vers':         self.version,
                'depth':        self.depth,
                'has_parent':   self.parent is not None,
                'parent':       self.parent,
                'has_children': self.has_children,
                'children':     self.children(),
                'stat':         str(self.status),
                'has_time':     self.elapsed is not None,
                'time':         self.elapsed,
                'has_start':    self.timestamp is not None,
                'start':        self.timestamp,
                'has_host':     self.hostname is not None,
                'host':         self.hostname,
                'pass_count':   self.pass_count,
                'pass_ratio':   self.pass_ratio,
                'fail_count':   self.fail_count,
                'fail_ratio':   self.fail_ratio,
                'skip_count':   self.skip_count,
                'skip_ratio':   self.skip_ratio,
                'excp_count':   self.excp_count,
                'excp_ratio':   self.excp_ratio,
                'off_count':    self.off_count,
                'off_ratio':    self.off_ratio,
                'exec_count':   self.exec_count,
                'exec_ratio':   self.exec_ratio,
                'total':        self.test_count,
                'has_path':     self.path is not None,
                'path':         self.path,
                'has_line':     self.line is not None,
                'line':         self.line,
        }
        
    @property
    def kind(self) -> str:
        return 'base'

    @property
    def parent(self) -> CuteSuite | None:
        return self._parent

    @property
    def depth(self) -> int:
        return self._depth

    @property
    def name(self) -> str:
        return self._name

    @property
    def full_name(self) -> str:
        if self._parent:
            return self._parent.full_name + '::' + self._name
        return self._name

    @property
    def status(self) -> CuteStatus:
        return self._status

    @property
    def elapsed(self) -> timedelta | None:
        return self._elapsed

    @property
    def timestamp(self) -> date | None:
        return self._timestamp

    @property
    def hostname(self) -> str | None:
        return self._hostname

    @property
    def version(self) -> str | None:
        return self._version

    @property
    def package(self) -> str | None:
        return self._package

    @property
    def path(self) -> str | None:
        return self._path

    @property
    def line(self) -> int | None:
        return self._line

    @property
    def test_count(self) -> int | None:
        return self._count

    @property
    def exec_count(self) -> int | None:
        return self._exec
    
    @property
    def exec_ratio(self) -> int | None:
        return self._ratio(self.exec_count, self.test_count)

    @property
    def off_count(self) -> int | None:
        return self._off

    @property
    def off_ratio(self) -> int | None:
        return self._ratio(self.off_count, self.test_count)

    @property
    def pass_count(self) -> int | None:
        return self._pass

    @property
    def pass_ratio(self) -> int | None:
        return self._ratio(self.pass_count, self.exec_count)

    @property
    def skip_count(self) -> int | None:
        return self._skip

    @property
    def skip_ratio(self) -> int | None:
        return self._ratio(self.skip_count, self.exec_count)

    @property
    def fail_count(self) -> int | None:
        return self._fail

    @property
    def fail_ratio(self) -> int | None:
        return self._ratio(self.fail_count, self.exec_count)

    @property
    def excp_count(self) -> int | None:
        return self._excp

    @property
    def excp_ratio(self) -> int | None:
        return self._ratio(self.excp_count, self.exec_count)

    @property
    def has_children(self) -> bool:
        return False
    
    def children(self) -> Iterator[CuteBase]:
        # Return an empty generator...
        yield from ()

    def accept(self, visit: CuteVisitor) -> None:
        raise AssertionError(self.__class__.__name__ +
                             '.accept() not implemented')
        
    @staticmethod
    def _ratio(count: int | None, total: int | None) -> int | None:
        if count is None or total is None:
            return None
        try:
            return round(count * 100 / total)
        except ZeroDivisionError:
            return 0

 
class CuteCase(CuteBase):
    def __init__(self, junit: JUnitTestCase, parent: CuteSuite) -> None:
        attrs = junit._elem.attrib
        status: str | CuteStatus = '??'
        if 'status' in attrs.keys():
            s = attrs['status']
            if s and len(s) > 0:
                status = s
        if status == '??':
            status = CuteStatus.NONE
        elif status == 'disabled':
            status = CuteStatus.OFF
        else:
            if junit.result:
                res = junit.result[0]
                if isinstance(res, JUnitSkipped):
                    status = CuteStatus.SKIP
                elif isinstance(res, JUnitFailure):
                    status = CuteStatus.FAIL
                elif isinstance(res, JUnitError):
                    status = CuteStatus.EXCP
                else:
                    status = CuteStatus.NONE
            else:
                    status = CuteStatus.PASS
        super().__init__(junit,
                         parent,
                         status,
                         parent.package,
                         parent.version,
                         parent.hostname)

    @property
    def kind(self) -> str:
        return 'case'
    
    def accept(self, visit: CuteVisitor) -> None:
        visit.case(self)


class CuteSuite(CuteBase):
    def __init__(self,
                 junit: JUnitTestSuite,
                 parent: CuteSuite | None) -> None:
        self._children: list[CuteBase] = []
        attrs = junit._elem.attrib
        total_cnt = int(junit.tests)
        off_cnt = 0
        if 'disabled' in attrs.keys():
            off_cnt = int(attrs['disabled'])
        if off_cnt != total_cnt:
            exec_cnt = total_cnt - off_cnt
            skip_cnt = int(junit.skipped)
            fail_cnt = int(junit.failures)
            excp_cnt = int(junit.errors)
            pass_cnt = exec_cnt - skip_cnt - fail_cnt - excp_cnt
            if skip_cnt == exec_cnt:
                status = CuteStatus.SKIP
            elif (pass_cnt + skip_cnt) == exec_cnt:
                status = CuteStatus.PASS
            else:
                status = CuteStatus.FAIL
        else:
            exec_cnt = 0
            skip_cnt = 0
            fail_cnt = 0
            excp_cnt = 0
            pass_cnt = 0
            status = CuteStatus.OFF
        hostname = None
        if 'hostname' in attrs.keys():
            host = attrs['hostname']
            if host and len(host) > 0:
                hostname = host
        package = None
        version = None
        if 'package' in attrs.keys():
            pkg = attrs['package']
            if pkg and len(pkg) > 0:
                package = pkg
            if 'version' in attrs.keys():
                vers = attrs['version']
                if vers and len(vers) > 0:
                    version = vers
        super().__init__(junit, parent, status, package, version, hostname)
        self._count = total_cnt
        self._off = off_cnt
        self._exec = exec_cnt
        self._skip = skip_cnt
        self._fail = fail_cnt
        self._excp = excp_cnt
        self._pass = pass_cnt
    
    @property
    def kind(self) -> str:
        return 'suite'

    @property
    def has_children(self) -> bool:
        return len(self._children) > 0

    def register(self, base: CuteBase) -> None:
        self._children.append(base)
   
    def children(self) -> Iterator[CuteBase]:
        for child in self._children:
            yield child

    def accept(self, visit: CuteVisitor) -> None:
        visit.suite(self)


class CuteStore(CuteBase):
    def __init__(self, junit: JUnitXml) -> None:
        self._suites = []
        if isinstance(junit, JUnitTestSuite):
            self._suites.append(self._load_junit_suite(junit, None))
        elif isinstance(junit, JUnitXml):
            for suite in junit:
                self._suites.append(self._load_junit_suite(suite, None))
        else:
            raise Exception('cannot load store: invalid specified object')
        self._name = 'Top-level store'
        self._elapsed = timedelta(0)
        self._timestamp = None
        self._package = None
        self._version = None
        self._hostname = None
        self._path = junit.filepath
        self._line = None
        self._parent = None
        self._depth = -1
        self._count = 0
        self._exec = 0
        self._off = 0
        self._pass = 0
        self._skip = 0
        self._fail = 0
        self._excp = 0
        for suite in self._suites:
            self._elapsed += suite.elapsed
            self._count += int(suite.test_count)
            self._exec += int(suite.exec_count)
            self._off += int(suite.off_count)
            self._pass += int(suite.pass_count)
            self._skip += int(suite.skip_count)
            self._fail += int(suite.fail_count)
            self._excp += int(suite.excp_count)
        if self._off != self._count:
            if self._skip == self._exec:
                self._status = CuteStatus.SKIP
            elif (self._pass + self._skip) == self._exec:
                self._status = CuteStatus.PASS
            else:
                self._status = CuteStatus.FAIL
        else:
            self._status = CuteStatus.OFF

    def _load_junit_suite(self,
                          junit: JUnitElement,
                          parent: JUnitTestSuite) -> CuteSuite:
        root = CuteSuite(junit, parent)
        for suite in junit.testsuites():
            self._load_junit_suite(suite, root)
        for case in junit.iterchildren(JUnitTestCase):
            CuteCase(case, root)
        return root

    @property
    def kind(self) -> str:
        return 'store'

    @property
    def has_children(self) -> bool:
        return len(self._suites) > 0
    
    def children(self) -> Iterator[CuteBase]:
        for suite in self._suites:
            yield suite

    def accept(self, visit: CuteVisitor) -> None:
        visit.store(self)


class CuteVisitor:
    def store(self, store: CuteStore) -> None:
        raise AssertionError(self.__class__.__name__ +
                             '.store() not implemented')
    
    def suite(self, suite: CuteSuite) -> None:
        raise AssertionError(self.__class__.__name__ +
                             '.suite() not implemented')

    def case(self, case: CuteCase) -> None:
        raise AssertionError(self.__class__.__name__ +
                             '.case() not implemented')


class CuteTraversal:
    def inorder(self,
                store: CuteStore,
                visit: CuteVisitor,
                select: str | None = None) -> None:
        def walk(base: CuteBase, visit: CuteVisitor, match) -> None:
            if match:
                ret = eval(match, None, base.expose)
                if not isinstance(ret, bool):
                    raise Exception('cannot traverse store: '
                                    'invalid selector expression: '
                                    'boolean return value expected')
            else:
                ret = True
            if ret:
                base.accept(visit)
            for child in base.children():
                walk(child, visit, match)

        if select:
            if len(select) == 0:
                raise Exception('cannot init store traversal: '
                                'empty selector expression')
            match = compile(select, '<string>', 'eval')
        else:
            match = None
        walk(store, visit, match)
    
    def find(self, store:CuteStore, name: str) -> CuteBase | None:
        def walk(base: CuteBase) -> CuteBase | None:
            if name == base.name or name == base.full_name:
                return base
            for child in base.children():
                found = walk(child)
                if found is not None:
                    return found
            return None

        return walk(store)


class CuteField:
    _IDENT: str
    _DESC: str
    
    @property
    def ident(self) -> str:
        return self._IDENT

    @property
    def head(self) -> str:
        return self.label.upper()

    @property
    def label(self) -> str:
        if hasattr(self, '_LABEL'):
            return self._LABEL
        return self.ident.capitalize()

    @property
    def desc(self) -> str:
        return self._DESC
    
    def render(self, base: CuteBase) -> Text | str:
        raise AssertionError(self.__class__.__name__ +
                             '.render() not implemented')
    
    def __gt__(self: CuteField, other: CuteField) -> bool:
        if not isinstance(other, CuteField):
            return NotImplemented
        return self.ident > other.ident

    def __ge__(self: CuteField, other: CuteField) -> bool:
        if not isinstance(other, CuteField):
            return NotImplemented
        return self.ident >= other.ident
    
    def __lt__(self: CuteField, other: CuteField) -> bool:
        if not isinstance(other, CuteField):
            return NotImplemented
        return self.ident < other.ident

    def __le__(self: CuteField, other: CuteField) -> bool:
        if not isinstance(other, CuteField):
            return NotImplemented
        return self.ident <= other.ident


class CuteRender(CuteField):
    _STYLE: Style | None = None

    def __init__(self, field: CuteField) -> None:
        self._field = field

    @property
    def ident(self) -> str:
        return self._field.ident

    @property
    def head(self) -> str:
        return self._field.head

    @property
    def label(self) -> str:
        return self._field.label

    @property
    def desc(self) -> str:
        return self._field.desc

    @property
    def style(self) -> Style | str:
        if self._STYLE is None:
            return ''
        return self._STYLE

    def render(self, base: CuteBase) -> Text | str:
        text = Text()
        text.append(self._field.render(base), style = self.style)
        return text

    def __gt__(self, other: CuteField) -> bool:
        return self._field.__gt__(other)

    def __ge__(self, other: CuteField) -> bool:
        return self._field.__ge__(other)

    def __lt__(self, other: CuteField) -> bool:
        return self._field.__lt__(other)

    def __le__(self, other: CuteField) -> bool:
        return self._field.__le__(other)


class CuteStoreRender(CuteRender):
    _STYLE = Style(color = 'bright_white')


class CuteSuiteRender(CuteRender):
    _STYLE = Style(color = 'blue')


class CuteRenderVisitor:
    def store(self, store: CuteStore) -> CuteRender:
        raise AssertionError(self.__class__.__name__ +
                             '.store() not implemented')

    def suite(self, suite: CuteSuite) -> CuteRender:
        raise AssertionError(self.__class__.__name__ +
                             '.suite() not implemented')

    def case(self, case: CuteCase) -> CuteRender:
        raise AssertionError(self.__class__.__name__ +
                             '.case() not implemented')


class CuteNameRenderVisitor(CuteVisitor):
    def __init__(self, field: CuteField) -> None:
        self._render: CuteRender
        self._base: CuteBase
        self._field = field

    def store(self, store: CuteStore) -> None:
        self._render = CuteStoreRender(self._field)
        self._base = store

    def suite(self, suite: CuteSuite) -> None:
        self._render = CuteSuiteRender(self._field)
        self._base = suite

    def case(self, case: CuteCase) -> None:
        self._render = CuteRender(self._field)
        self._base = case

    def render(self) -> Text | str:
        return self._render.render(self._base)


class CuteNameRender(CuteRender):
    def render(self, base: CuteBase) -> Text | str:
        visit = CuteNameRenderVisitor(self._field)
        base.accept(visit)
        return visit.render()


class CuteParentRender(CuteSuiteRender):
    def render(self, base: CuteBase) -> Text | str:
        parent = base.parent
        if parent is None:
            return 'none'
        return super().render(base)


class CuteNameField(CuteField):
    _IDENT = 'name'
    _DESC = 'short name of test case / suite'

    @staticmethod
    def render(base: CuteBase) -> str:
        return base.name


class CuteTypeField(CuteField):
    _IDENT = 'type'
    _DESC = 'either \'suite\' or \'case\''

    @staticmethod
    def render(base: CuteBase) -> str:
        return base.kind


class CuteOptionalField(CuteField):
    @staticmethod
    def _render(value) -> str:
        if value is not None:
            return str(value)
        else:
            return '??'


class CutePathField(CuteOptionalField):
    _IDENT = 'path'
    _DESC = 'pathname to source file of test case / suite'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.path)


class CuteLineField(CuteOptionalField):
    _IDENT = 'line'
    _DESC = 'line within source file of test case / suite'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.line)


class CutePackageField(CuteOptionalField):
    _IDENT = 'pkg'
    _LABEL = 'Package'
    _DESC = 'package name of test case / suite'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.package)


class CuteVersionField(CuteOptionalField):
    _IDENT = 'vers'
    _LABEL = 'Version'
    _DESC = 'version of test cases / suites'
    
    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.version)


class CuteParentField(CuteField):
    _IDENT = 'parent'
    _DESC = 'test case / suite parent'

    @staticmethod
    def render(base: CuteBase) -> str:
        if base.parent is not None:
            return base.parent.name
        return 'none'


class CuteDepthField(CuteField):
    _IDENT = 'depth'
    _DESC = 'test case / suite depth'

    @staticmethod
    def render(base: CuteBase) -> str:
        return str(base.depth)


class CuteFullNameField(CuteField):
    _IDENT = 'full_name'
    _LABEL = 'Full name'
    _DESC = 'full name of test case / suite'
    _WRAP = 'True'
    
    @staticmethod
    def render(base: CuteBase) -> str:
        return base.full_name


class CuteSourceField(CuteField):
    _IDENT = 'src'
    _LABEL = 'Source'
    _DESC = 'source file pathname and line number of test case / suite'

    @staticmethod
    def render(base: CuteBase) -> str:
        if base.path is None:
            return '??'
        if base.line is not None:
            line = str(base.line)
        else:
            line = '??'
        return '{}:{}'.format(base.path, line)


class CuteStatusRender(CuteRender):
    PASS_STYLE = Style(color = 'green')
    SKIP_STYLE = Style(color = 'yellow')
    FAIL_STYLE = Style(color = 'red')
    EXCP_STYLE = Style(color = 'red')
    OFF_STYLE = Style(color = 'bright_black', bold = True)

    def render(self, base: CuteBase) -> Text | str:
        text = Text()
        text.append(self._field.render(base), style = self._style(base.status))
        return text
    
    @classmethod
    def _style(cls, status: CuteStatus) -> Style:
        if status == CuteStatus.PASS:
            return cls.PASS_STYLE
        elif status == CuteStatus.SKIP:
            return cls.SKIP_STYLE
        elif status == CuteStatus.FAIL:
            return cls.FAIL_STYLE
        elif status == CuteStatus.EXCP:
            return cls.EXCP_STYLE
        elif status == CuteStatus.OFF:
            return cls.OFF_STYLE
        else:
            raise Exception("field style not found: "
                            "unsupported status: '{}'".format(str(status)))


class CutePassRender(CuteRender):
    _STYLE = CuteStatusRender.PASS_STYLE


class CuteSkipRender(CuteRender):
    _STYLE = CuteStatusRender.SKIP_STYLE


class CuteFailRender(CuteRender):
    _STYLE = CuteStatusRender.FAIL_STYLE


class CuteExcpRender(CuteRender):
    _STYLE = CuteStatusRender.EXCP_STYLE


class CuteOffRender(CuteRender):
    _STYLE = CuteStatusRender.OFF_STYLE


class CuteStatusField(CuteField):
    _IDENT = 'stat'
    _LABEL = 'Status'
    _DESC = 'status of test case / suite'

    @staticmethod
    def render(base: CuteBase) -> str:
        return str(base.status)


class CuteHostField(CuteOptionalField):
    _IDENT = 'host'
    _LABEL = 'Hostname'
    _DESC = 'name of host that ran test case / suite'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        if base.status != CuteStatus.OFF:
            return cls._render(base.hostname)
        return 'NA'


class CuteStartField(CuteOptionalField):
    _IDENT = 'start'
    _DESC = 'start date expressed according to local time settings'
    _WRAP = True

    @classmethod
    def render(cls, base: CuteBase) -> str:
        if base.status != CuteStatus.OFF:
            return cls._render(base.timestamp)
        return 'NA'


class CuteElapsedField(CuteOptionalField):
    _IDENT = 'time'
    _LABEL = 'Elapsed'
    _DESC = 'elapsed running time expressed as' \
            ' <hours>:<minutes>:<seconds>.<microseconds>'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        if base.status != CuteStatus.OFF:
            return cls._render(base.elapsed)
        return 'NA'
    
    @staticmethod
    def _render(value) -> str:
        if value is not None:
            hrs, secs = divmod(value.total_seconds(), 3600)
            mns, secs = divmod(secs, 60)
            return '{:02}:{:02}:{:02}.{:06}'.format(int(hrs),
                                                    int(mns),
                                                    int(secs),
                                                    value.microseconds)
        else:
            return '??'


class CuteCountField(CuteField):
    @staticmethod
    def _render(count: int | None) -> str:
        if count is None:
            return 'NA'
        return str(count)


class CutePassField(CuteCountField):
    _IDENT = 'pass_count'
    _LABEL = '#Pass'
    _DESC = 'absolute number of passed test cases'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.pass_count)


class CutePassRatioField(CuteCountField):
    _IDENT = 'pass_ratio'
    _LABEL = '%Pass'
    _DESC = 'ratio of passed test cases'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.pass_ratio)


class CuteSkipField(CuteCountField):
    _IDENT = 'skip_count'
    _LABEL = '#Skip'
    _DESC = 'absolute number of skipped test cases'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.skip_count)


class CuteSkipRatioField(CuteCountField):
    _IDENT = 'skip_ratio'
    _LABEL = '%Skip'
    _DESC = 'ratio of skipped test cases'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.skip_ratio)


class CuteFailField(CuteCountField):
    _IDENT = 'fail_count'
    _LABEL = '#Fail'
    _DESC = 'absolute number of failed test cases'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.fail_count)


class CuteFailRatioField(CuteCountField):
    _IDENT = 'fail_ratio'
    _LABEL = '%Fail'
    _DESC = 'ratio of failed test cases'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.fail_ratio)


class CuteExcpField(CuteCountField):
    _IDENT = 'excp_count'
    _LABEL = '#Excp'
    _DESC = 'absolute number of executed test cases'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.excp_count)


class CuteExcpRatioField(CuteCountField):
    _IDENT = 'excp_ratio'
    _LABEL = '%Excp'
    _DESC = 'ratio of executed test cases'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.excp_ratio)


class CuteOffField(CuteCountField):
    _IDENT = 'off_count'
    _LABEL = '#Off'
    _DESC = 'absolute number of disabled test cases'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.off_count)


class CuteOffRatioField(CuteCountField):
    _IDENT = 'off_ratio'
    _LABEL = '%Off'
    _DESC = 'ratio of disabled test cases'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.off_ratio)

class CuteExecField(CuteCountField):
    _IDENT = 'exec_count'
    _LABEL = '#Exec'
    _DESC = 'absolute number of crashed test cases'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.exec_count)


class CuteExecRatioField(CuteCountField):
    _IDENT = 'exec_ratio'
    _LABEL = '%Exec'
    _DESC = 'ratio of crashed test cases'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.exec_ratio)


class CuteTotalField(CuteCountField):
    _IDENT = 'total'
    _LABEL = '#Total'
    _DESC = 'absolute number of test cases'

    @classmethod
    def render(cls, base: CuteBase) -> str:
        return cls._render(base.test_count)
    

class CuteFieldFabric:
    _instance: CuteFieldFabric | None = None
    _fields = frozenset({
            CuteNameField(),
            CuteTypeField(),
            CutePathField(),
            CuteLineField(),
            CutePackageField(),
            CuteVersionField(),
            CuteParentField(),
            CuteDepthField(),
            CuteFullNameField(),
            CuteSourceField(),
            CuteStatusField(),
            CuteHostField(),
            CuteStartField(),
            CuteElapsedField(),
            CutePassField(),
            CutePassRatioField(),
            CuteSkipField(),
            CuteSkipRatioField(),
            CuteFailField(),
            CuteFailRatioField(),
            CuteExcpField(),
            CuteExcpRatioField(),
            CuteOffField(),
            CuteOffRatioField(),
            CuteTotalField(),
            CuteExecField(),
            CuteExecRatioField()
    })
    _dict: dict[str, CuteField] = dict()
    
    def __new__(cls) -> CuteFieldFabric:
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            for f in cls._instance._fields:
                cls._instance._dict[f.ident] = f
        return cls._instance
    
    def get(self, ident: str) -> CuteField:
        if ident in self._dict:
            return self._dict[ident]
        for f in self._fields:
            if ident == f.ident:
                self._dict[ident] = f
                return f
        raise Exception("invalid specified field: '{}'".format(ident))


class CuteSumField(CuteRender):
    _FIELD: str
    _JUSTIFY: Literal['default', 'left', 'center', 'right', 'full'] = 'left'
    _WRAP = False

    def __init__(self) -> None:
        super().__init__(CuteFieldFabric().get(self._FIELD))

    def column(self) -> CuteTableColumn:
        return CuteTableColumn(header = self.head,
                               justify = self._JUSTIFY,
                               wrap = self._WRAP)

    def store(self, store: CuteStore) -> RenderableType:
        return self.render(store)

    def suite(self, suite: CuteSuite) -> RenderableType:
        return self.render(suite)

    def case(self, case: CuteCase) -> RenderableType:
        return self.render(case)


class CuteNameSumField(CuteSumField):
    _FIELD = 'name'

    def store(self, store: CuteStore) -> RenderableType:
        return CuteStoreRender(self._field).render(store)

    def suite(self, suite: CuteSuite) -> RenderableType:
        return self._render(suite, CuteSuiteRender(self._field).render(suite))

    def case(self, case: CuteCase) -> RenderableType:
        return self._render(case, self.render(case))

    @staticmethod
    def _render(base: CuteBase, renderable: Text | str) -> Text:
        text = Text('{:<{}}'.format('', 3 * base.depth))
        text.append(renderable)
        return text


class CuteFullNameSumField(CuteNameSumField):
    _FIELD = 'full_name'
    _WRAP = True


class CuteStatusSumField(CuteSumField):
    _FIELD = 'stat'
    _JUSTIFY = 'center'

    def render(self, base: CuteBase) -> Text | str:
        return CuteStatusRender(self._field).render(base)


class CuteTypeSumField(CuteSumField):
    _FIELD = 'type'
    _JUSTIFY = 'center'


class CutePackageSumField(CuteSumField):
    _FIELD = 'pkg'
    _JUSTIFY = 'center'
    _WRAP = True


class CuteVersionSumField(CuteSumField):
    _FIELD = 'vers'
    _JUSTIFY = 'center'


class CuteDepthSumField(CuteSumField):
    _FIELD = 'depth'
    _JUSTIFY = 'right'


class CuteParentSumField(CuteSumField):
    _FIELD = 'parent'
    _JUSTIFY = 'center'

    def render(self, base: CuteBase) -> Text | str:
        return CuteParentRender(self._field).render(base)


class CuteHostSumField(CuteSumField):
    _FIELD = 'host'
    _JUSTIFY = 'center'


class CuteStartSumField(CuteSumField):
    _FIELD = 'start'
    _JUSTIFY = 'center'
    _WRAP = True


class CuteElapsedSumField(CuteSumField):
    _FIELD = 'time'
    _JUSTIFY = 'right'
    

class CutePathSumField(CuteSumField):
    _FIELD = 'path'
    _WRAP = True


class CuteLineSumField(CuteSumField):
    _FIELD = 'line'
    _JUSTIFY = 'right'


class CuteSourceSumField(CuteSumField):
    _FIELD = 'src'
    _WRAP = True


class CuteStatSumField(CuteSumField):
    _JUSTIFY = 'right'
     
    def __init__(self):
        super().__init__()
        if self._RENDER is not None:
            self._high_render = self._RENDER(self._field)
        else:
            self._high_render = self._field
        
    def store(self, store: CuteStore) -> RenderableType:
        return self._high_render.render(store)
   
    def render(self, base: CuteBase) -> Text | str:
        if base.status != CuteStatus.OFF:
            return self._field.render(base)
        return 'NA'


class CutePassCountSumField(CuteStatSumField):
    _FIELD = 'pass_count'
    _RENDER = CutePassRender


class CutePassRatioSumField(CuteStatSumField):
    _FIELD = 'pass_ratio'
    _RENDER = CutePassRender


class CuteSkipCountSumField(CuteStatSumField):
    _FIELD = 'skip_count'
    _RENDER = CuteSkipRender


class CuteSkipRatioSumField(CuteStatSumField):
    _FIELD = 'skip_ratio'
    _RENDER = CuteSkipRender


class CuteFailCountSumField(CuteStatSumField):
    _FIELD = 'fail_count'
    _RENDER = CuteFailRender


class CuteFailRatioSumField(CuteStatSumField):
    _FIELD = 'fail_ratio'
    _RENDER = CuteFailRender


class CuteExcpCountSumField(CuteStatSumField):
    _FIELD = 'excp_count'
    _RENDER = CuteExcpRender


class CuteExcpRatioSumField(CuteStatSumField):
    _FIELD = 'excp_ratio'
    _RENDER = CuteExcpRender


class CuteOffCountSumField(CuteStatSumField):
    _FIELD = 'off_count'
    _RENDER = CuteOffRender


class CuteOffRatioSumField(CuteStatSumField):
    _FIELD = 'off_ratio'
    _RENDER = CuteOffRender


class CuteExecCountSumField(CuteStatSumField):
    _FIELD = 'exec_count'
    _RENDER = None


class CuteExecRatioSumField(CuteStatSumField):
    _FIELD = 'exec_ratio'
    _RENDER = None


class CuteTotalSumField(CuteStatSumField):
    _FIELD = 'total'
    _RENDER = None


class CuteConsole(Console):
    def __init__(self, color: Literal['on', 'off', 'auto'] = 'auto') -> None:
        if color == 'on':
            tty = True
        elif color == 'off':
            tty = False
        elif color == 'auto':
            tty = None
        else:
            raise Exception("cannot init console: "
                            "invalid color mode: '{}'".format(color))
        super().__init__(force_terminal = tty)

    def show(self, renderable: ConsoleRenderable) -> None:
        self.print(renderable)


class CuteSectionTitle(Text):
    def __init__(self,
                 title: str,
                 justify: Literal['default',
                                  'left',
                                  'center',
                                  'right',
                                  'full'] = 'left') -> None:
        super().__init__(justify = justify)
        self.append('### ')
        self.append(title, style = Style(bold = True))
        self.append(' ###\n')
        self._width = len(title) + 8
        
    @property
    def width(self) -> int:
        return self._width
    
    
class CuteTableColumn(Column):
    def __init__(self,
                 header: str = '',
                 footer: RenderableType = '',
                 justify: Literal['default',
                                  'left',
                                  'center',
                                  'right',
                                  'full'] = 'left',
                 wrap: bool = False) -> None:
        super().__init__(header = header,
                         footer = footer,
                         justify = justify,
                         no_wrap = not wrap)


class CuteTable(Table):
    def __init__(self,
                 *columns: Column,
                 title: str,
                 show_header: bool = False,
                 padding: PaddingDimensions = (0, 0, 0, 1)) -> None:
        ttl = CuteSectionTitle(title)
        super().__init__(*columns,
                         title = ttl,
                         title_justify = 'left',
                         box = None,
                         show_header = show_header,
                         show_edge = False,
                         pad_edge = False,
                         min_width = ttl.width,
                         padding = padding)


class CuteTableSection(CuteVisitor):
    def __init__(self,
                 *columns: Column,
                 title: str,
                 show_header: bool = False,
                 padding: PaddingDimensions = (0, 0, 0, 1)) -> None:
        self._table = CuteTable(*columns,
                                title = title,
                                show_header = show_header,
                                padding = padding)

    def add_row(self,
                *renderables: RenderableType, 
                style: Style | None = None) -> None:
        self._table.add_row(*renderables, style = style)
    
    def __rich_console__(self,
                         console: Console,
                         options: ConsoleOptions) -> RenderResult:
        yield self._table
    

class CuteSumSection(CuteTableSection):
    _known: dict[str, CuteSumField] = dict()
    _fields = (
            CuteNameSumField(),
            CuteFullNameSumField(),
            CuteStatusSumField(),
            CuteTypeSumField(),
            CutePackageSumField(),
            CuteVersionSumField(),
            CuteDepthSumField(),
            CuteParentSumField(),
            CuteHostSumField(),
            CuteStartSumField(),
            CuteElapsedSumField(),
            CutePassCountSumField(),
            CutePassRatioSumField(),
            CuteSkipCountSumField(),
            CuteSkipRatioSumField(),
            CuteFailCountSumField(),
            CuteFailRatioSumField(),
            CuteExcpCountSumField(),
            CuteExcpRatioSumField(),
            CuteOffCountSumField(),
            CuteOffRatioSumField(),
            CuteExecCountSumField(),
            CuteExecRatioSumField(),
            CuteTotalSumField(),
            CutePathSumField(),
            CuteLineSumField(),
            CuteSourceSumField()
    )

    def __init__(self, idents: list[str]) -> None:
        if len(idents) == 0:
            raise Exception('cannot init report: empty specified field list')
        if len(self._known) == 0:
            for f in self._fields:
                self._known[f.ident] = f
        for i in idents:
            if i not in self._known:
                raise Exception("cannot init report: "
                                "invalid specified field: '{}'".format(i))
        self._visible = idents
        self._store: CuteStore | None = None
        super().__init__(*tuple(self._known[i].column() for i in self._visible),
                         title = 'Summary',
                         show_header = True)

    @classmethod
    def fields(cls) -> Iterator[CuteField]:
        for f in cls._fields:
            yield f

    def store(self, store: CuteStore) -> None:
        self._store = store
    
    def suite(self, suite: CuteSuite) -> None:
        self.add_row(*tuple(self._known[f].suite(suite) for f in self._visible))

    def case(self, case: CuteCase):
        self.add_row(*tuple(self._known[f].case(case) for f in self._visible))

    def __rich_console__(self,
                         console: Console,
                         options: ConsoleOptions) -> RenderResult:
        if self._store is not None:
            self.add_row(*tuple(self._known[f].store(self._store)
                                for f in self._visible),
                         style = Style(bold = True))
        return super().__rich_console__(console, options)


class CuteLabelRender:
    @classmethod
    def render(cls, field: CuteField) -> RenderableType:
        return cls._render(field.label)
    
    @staticmethod
    def _render(label: str) -> RenderableType:
        return Text(':' + label + ':', style = Style(bold = True))

   
class CuteFieldSection(CuteTableSection):
    def __init__(self, title: str) -> None:
        self._labeller = CuteLabelRender()
        super().__init__(CuteTableColumn(),
                         CuteTableColumn(wrap = True),
                         title = title)

    def _fill(self, base: CuteBase, *fields: CuteField) -> None:
        for f in fields:
            self.add_row(self._labeller.render(f), f.render(base))


class CuteDescSection(CuteFieldSection):
    def __init__(self) -> None:
        super().__init__('Description')

    def store(self, store: CuteStore) -> None:
        self._fill(store,
                   CuteStoreRender(CuteFieldFabric().get('name')),
                   CuteFieldFabric().get('type'),
                   CuteFieldFabric().get('path'))

    def suite(self, suite: CuteSuite) -> None:
        self._fill(suite,
                   CuteSuiteRender(CuteFieldFabric().get('name')),
                   CuteFieldFabric().get('pkg'),
                   CuteFieldFabric().get('vers'),
                   CuteFieldFabric().get('type'),
                   CuteParentRender(CuteFieldFabric().get('parent')),
                   CuteFieldFabric().get('depth'),
                   CuteSuiteRender(CuteFieldFabric().get('full_name')),
                   CuteFieldFabric().get('src'))

    def case(self, case: CuteCase) -> None:
        self._fill(case,
                   CuteFieldFabric().get('name'),
                   CuteFieldFabric().get('pkg'),
                   CuteFieldFabric().get('vers'),
                   CuteFieldFabric().get('type'),
                   CuteParentRender(CuteFieldFabric().get('parent')),
                   CuteFieldFabric().get('depth'),
                   CuteFieldFabric().get('full_name'),
                   CuteFieldFabric().get('src'))


class CuteRunSection(CuteFieldSection):
    def __init__(self) -> None:
        super().__init__('Run')

    def store(self, store: CuteStore) -> None:
        self._fill(store,
                   CuteStatusRender(CuteFieldFabric().get('stat')),
                   CuteFieldFabric().get('time'))

    def suite(self, suite: CuteSuite) -> None:
        self._fill_base(suite)

    def case(self, case: CuteCase) -> None:
        self._fill_base(case)

    def _fill_base(self, base: CuteBase) -> None:
        self._fill(base,
                   CuteStatusRender(CuteFieldFabric().get('stat')),
                   CuteFieldFabric().get('host'),
                   CuteFieldFabric().get('start'),
                   CuteFieldFabric().get('time'))


class CuteStatLabelRender(CuteLabelRender):
    @classmethod
    def render(cls, field: CuteField) -> RenderableType:
        return cls._render(field.label[1:].capitalize())


class CuteStatRatioRender(CuteRender):
    def render(self, base: CuteBase) -> Text | str:
        text = Text('(')
        text.append(self._field.render(base))
        text.append('%)')
        return text


class CuteStatSection(CuteTableSection):
    def __init__(self) -> None:
        self._labeller = CuteStatLabelRender()
        super().__init__(CuteTableColumn(),
                         CuteTableColumn(),
                         CuteTableColumn(justify = 'right'),
                         CuteTableColumn(justify = 'center'),
                         CuteTableColumn(justify = 'right'),
                         CuteTableColumn(),
                         CuteTableColumn(justify = 'right'),
                         CuteTableColumn(justify = 'center'),
                         CuteTableColumn(),
                         CuteTableColumn(),
                         CuteTableColumn(justify = 'right'),
                         CuteTableColumn(justify = 'center'),
                         CuteTableColumn(justify = 'right'),
                         CuteTableColumn(),
                         CuteTableColumn(justify = 'right'),
                         title = 'Statistics', 
                         padding = (0, 0))
        
    def store(self, store: CuteStore) -> None:
        self._fill(store)
    
    def suite(self, suite: CuteSuite) -> None:
        self._fill(suite)
    
    def _fill(self, base: CuteBase) -> None:
        exec_cnt = CuteFieldFabric().get('exec_count')
        self._fill_row(base,
                       CuteOffRender(CuteFieldFabric().get('off_count')),
                       CuteOffRender(CuteFieldFabric().get('off_ratio')),
                       exec_cnt,
                       CuteFieldFabric().get('exec_ratio'),
                       CuteFieldFabric().get('total'))
        self._fill_row(base,
                       CutePassRender(CuteFieldFabric().get('pass_count')),
                       CutePassRender(CuteFieldFabric().get('pass_ratio')),
                       CuteSkipRender(CuteFieldFabric().get('skip_count')),
                       CuteSkipRender(CuteFieldFabric().get('skip_ratio')),
                       exec_cnt)
        self._fill_row(base,
                       CuteFailRender(CuteFieldFabric().get('fail_count')),
                       CuteFailRender(CuteFieldFabric().get('fail_ratio')),
                       CuteExcpRender(CuteFieldFabric().get('excp_count')),
                       CuteExcpRender(CuteFieldFabric().get('excp_ratio')),
                       exec_cnt)

    def _fill_row(self,
                  base: CuteBase,
                  cnt0: CuteField,
                  ratio0: CuteField,
                  cnt1: CuteField,
                  ratio1: CuteField,
                  total: CuteField) -> None:
        self.add_row(self._labeller.render(cnt0),
                     ' ',
                     cnt0.render(base),
                     '/',
                     total.render(base),
                     ' ',
                     CuteStatRatioRender(ratio0).render(base),
                     '  ',
                     self._labeller.render(cnt1),
                     ' ',
                     cnt1.render(base),
                     '/',
                     total.render(base),
                     ' ',
                     CuteStatRatioRender(ratio1).render(base))


class CuteChildSection(Columns):
    def __init__(self) -> None:
        ttl = CuteSectionTitle('Children')
        super().__init__(align = 'left',
                         padding = (0, 0, 0, 2),
                         column_first = False,
                         expand = False,
                         width = ttl.width,
                         title = ttl)

    def store(self, store: CuteStore) -> None:
        self._fill(store)
    
    def suite(self, suite: CuteSuite) -> None:
        self._fill(suite)

    def _fill(self, base: CuteBase) -> None:
        children = list(base.children())
        if len(children) > 0:
            for c in children:
                self.width: int
                width = len(c.name)
                if width > self.width:
                    self.width = width
                self.add_renderable(
                    CuteNameRender(CuteFieldFabric().get('name')).render(c))
        else:
            self.add_renderable('none')
    

class CuteResultSection(Text):
    def __init__(self) -> None:
        super().__init__()

    def store(self, store: CuteStore) -> None:
        self._fill(store)
    
    def suite(self, suite: CuteSuite) -> None:
        self._fill(suite)

    def case(self, case: CuteCase) -> None:
        self._append(*self._message(case))
        
    def _fill(self, base: CuteBase) -> None:
        msg, style = self._message(base)
        if base.status == CuteStatus.FAIL or base.status == CuteStatus.EXCP:
            assert isinstance(base.fail_count, int)
            assert isinstance(base.excp_count, int)
            assert isinstance(base.exec_count, int)
            cnt = base.fail_count + base.excp_count
            msg = '{}/{} ({}%) FAILED'.format(cnt,
                                              base.exec_count,
                                              round(cnt * 100 /
                                                    base.exec_count))
        self._append(msg, style)
            
    def _message(self, base: CuteBase) -> tuple[str, Style]:
        if base.status == CuteStatus.PASS:
            status = 'PASSED'
            style = CuteStatusRender.PASS_STYLE
        elif base.status == CuteStatus.SKIP:
            status = 'SKIPPED'
            style = CuteStatusRender.SKIP_STYLE
        elif base.status == CuteStatus.FAIL:
            status = 'FAILED'
            style = CuteStatusRender.FAIL_STYLE
        elif base.status == CuteStatus.EXCP:
            status = 'CRASHED'
            style = CuteStatusRender.EXCP_STYLE
        elif base.status == CuteStatus.OFF:
            status = 'DISABLED'
            style = CuteStatusRender.OFF_STYLE
        else:
            raise Exception("cannot setup result section: "
                            "invalid status: '{}'".format(base.status))
        return (status, style)
    
    def _append(self, text: str, style: Style) -> None:
        self.append('### ')
        self.append(text, style = style + Style(bold = True))
        self.append(' ###')


class CuteFieldReport(CuteTable):
    def __init__(self) -> None:
        super().__init__(CuteTableColumn(header = 'IDENTIFIER'),
                         CuteTableColumn(header = 'LABEL'),
                         CuteTableColumn(header = 'DESCRIPTION'),
                         title = 'Available fields',
                         show_header = True)
        for field in sorted(CuteSumSection.fields()):
            self.add_row(Text(field.ident, style = Style(bold = True)),
                         field.label,
                         field.desc)


class CuteSumReport:
    def __init__(self,
                 store: CuteStore,
                 traverse: CuteTraversal,
                 fields: str | None = None,
                 select: str | None = None) -> None:
        if fields is None:
            field_list = ['name',
                          'pkg',
                          'time',
                          'pass_count',
                          'skip_count',
                          'fail_count',
                          'excp_count',
                          'exec_count',
                          'total',
                          'stat']
        else:
            field_list = fields.split(',')
        if select is None:
            select = "type in ('store', 'suite')"
        elif select == 'all':
            select = 'True'
        self._sumup = CuteSumSection(field_list)
        traverse.inorder(store, self._sumup, select)
        self._res = CuteResultSection()
        self._res.store(store)
        
    def __rich_console__(self,
                         console: Console,
                         options: ConsoleOptions) -> RenderResult:
        yield self._sumup
        yield Padding(self._res, pad = (1, 0, 0, 0))
 
 

class CuteResultReport(CuteVisitor):
    def __init__(self) -> None:
        self._stat: CuteStatSection | None
    
    def store(self, store: CuteStore) -> None:
        self._desc = CuteDescSection()
        self._desc.store(store)
        self._run = CuteRunSection()
        self._run.store(store)
        self._stat = CuteStatSection()
        self._stat.store(store)
        self._res = CuteResultSection()
        self._res.store(store)
    
    def suite(self, suite: CuteSuite) -> None:
        self._desc = CuteDescSection()
        self._desc.suite(suite)
        self._run = CuteRunSection()
        self._run.suite(suite)
        self._stat = CuteStatSection()
        self._stat.suite(suite)
        self._res = CuteResultSection()
        self._res.suite(suite)
    
    def case(self, case: CuteCase) -> None:
        self._desc = CuteDescSection()
        self._desc.case(case)
        self._run = CuteRunSection()
        self._run.case(case)
        self._stat = None
        self._res = CuteResultSection()
        self._res.case(case)
    
    def __rich_console__(self,
                         console: Console,
                         options: ConsoleOptions) -> RenderResult:
        yield self._desc
        yield Padding(self._run, pad = (1, 0, 0, 0))
        if self._stat:
            yield Padding(self._stat, pad = (1, 0, 0, 0))
        yield Padding(self._res, pad = (1, 0, 0, 0))
 

class CuteInfoReport(CuteVisitor):
    def __init__(self) -> None:
        self._child: CuteChildSection | None
    
    def store(self, store: CuteStore) -> None:
        self._desc = CuteDescSection()
        self._desc.store(store)
        self._child = CuteChildSection()
        self._child.store(store)
         
    def suite(self, suite: CuteSuite) -> None:
        self._desc = CuteDescSection()
        self._desc.suite(suite)
        self._child = CuteChildSection()
        self._child.suite(suite)
    
    def case(self, case: CuteCase) -> None:
        self._desc = CuteDescSection()
        self._desc.case(case)
        self._child = None
    
    def __rich_console__(self,
                         console: Console,
                         options: ConsoleOptions) -> RenderResult:
        yield self._desc
        if self._child:
            yield Padding(self._child, pad = (1, 0, 0, 0))


class CuteDB:
    def __init__(self, path: str) -> None:
        try:
            db = JUnitXml.fromfile(path)
        except FileNotFoundError:
            db = JUnitXml()
            db.filepath = path
        if isinstance(db, JUnitTestSuite):
            self._db = JUnitXml()
            self._db.add_testsuite(db)
            self._db.update_statistics()
        elif isinstance(db, JUnitXml):
            self._db = db
        else:
            raise Exception("cannot open '{}' JUnit database: "
                            "unexpected JUnit file format".format(path))

    def insert(self, parent: str | None, path: str, name: str | None) -> None:
        try:
            (junit, elders, child, name) = self._cook_insert(parent, path, name)
            if child is not None:
                raise Exception(
                    "element '{}::{}' already exists".format(parent, name))
            self._add(junit, name, elders)
        except Exception as e:
            raise Exception("cannot insert into '{}' JUnit database: "
                            "{}".format(self._db.filepath, e))

    def update(self, parent: str | None, path: str, name: str | None) -> None:
        try:
            (junit, elders, child, name) = self._cook_insert(parent, path, name)
            if child is not None:
                elders[-1].remove(child)
                elders[-1].update_statistics()
            self._add(junit, name, elders)
        except Exception as e:
            raise Exception("cannot update '{}' JUnit database: "
                            "{}".format(self._db.filepath, e))

    def delete(self, full_name: str) -> None:
        try:
            path = self._mkpath(full_name)
            if len(path) == 0:
                raise Exception("element unspecified")
            elders = self._walk(path)
            if elders is None:
                raise Exception("element '{}' not found".format(full_name))
            child = elders.pop()
            elders[-1].remove(child)
            self._refresh_stats(elders)
        except Exception as e:
            raise Exception("cannot delete from '{}' JUnit database: "
                            "{}".format(self._db.filepath, e))
    
    # FIXME: block unwanted signals while removing temporary files...
    def save(self) -> None:
        path = self._db.filepath
        (tmpfile, tmppath) = mkstemp(prefix = basename(path),
                                     dir = dirname(path),
                                     text = True) 
        try:
            close(tmpfile)
            self._db.write(tmppath)
            replace(tmppath, path)
        except Exception as e:
            unlink(tmppath)
            raise e
        sync()

    def _cook_insert(self,
                     parent: str | None,
                     path: str,
                     name: str | None) -> tuple[JUnitXml,
                                                list[JUnitElement],
                                                JUnitElement,
                                                str | None]:
        junit = JUnitXml.fromfile(path)
        if not isinstance(junit, JUnitXml) and \
           not isinstance(junit, JUnitTestSuite):
            raise Exception("unexpected '{}' JUnit file format".format(path))
        if name is None:
            name = junit.name
            if name is None:
                raise Exception('missing element name')
        elders = self._walk(self._mkpath(parent))
        if elders is None:
            raise Exception(
                "parent element '{}' not found".format(parent))
        child = self._find_child(elders[-1], name)
        return (junit, elders, child, name)
    
    @staticmethod
    def _mkpath(full_name: str | None) -> list[str]:
        if full_name is None or len(full_name) == 0 or full_name == '::':
            return []
        path = full_name.split('::')
        if len(path[0]) == 0:
            path.pop(0)
        for n in path:
            if len(n) == 0:
                raise Exception(
                    "invalid element full name '{}'".format(full_name))
        return path
    
    def _walk(self, path: list[str]) -> list[JUnitElement] | None:
        walk = [self._db]
        for n in path:
            curr = self._find_child(walk[-1], n)
            if curr is None:
                return None
            walk.append(curr)
        return walk

    @staticmethod
    def _find_child(parent: JUnitElement, name: str) -> JUnitElement | None:
        for elem in parent.iterchildren(JUnitTestSuite):
            if elem.name == name:
                return elem
        for elem in parent.iterchildren(JUnitTestCase):
            if elem.name == name:
                return elem
        return None

    def _add(self,
             junit: JUnitXml,
             name: str,
             elders: list[JUnitElement]) -> None:
        if isinstance(junit, JUnitXml):
            suite = JUnitTestSuite()
            props = junit.child(JUnitProperties)
            if props is not None:
                suite.append(props)
            for s in junit.iterchildren(JUnitTestSuite):
                suite.add_testsuite(s)
            suite.update_statistics()
        elif isinstance(junit, JUnitTestSuite):
            suite = junit
        else:
            raise Exception(
                "unexpected '{}' JUnit file format".format(junit.filepath))
        suite.name = name
        elders[-1].add_testsuite(suite)
        self._refresh_stats(elders)
        
    @staticmethod
    def _refresh_stats(path: list[JUnitElement]) -> None:
        assert len(path) > 0
        while len(path) > 0:
            elem = path.pop()
            elem.update_statistics()


def cute_verify(path: str) -> int:
    store = CuteStore(JUnitXml.fromfile(path))
    return int(store.status == CuteStatus.FAIL)


def cute_list(color: Literal['on', 'off', 'auto']) -> None:
    CuteConsole(color).show(CuteFieldReport())


def cute_sumup(path: str,
               fields: str,
               select: str,
               color: Literal['on', 'off', 'auto']) -> None:
    CuteConsole(color).show(CuteSumReport(CuteStore(JUnitXml.fromfile(path)),
                                          CuteTraversal(),
                                          fields,
                                          select))


def cute_join(db_path: str,
              junit_path: str,
              parent: str | None = None,
              name: str | None = None) -> None:
    db = CuteDB(db_path)
    db.update(parent, junit_path, name)
    db.save()


def cute_delete(db_path: str, full_name: str) -> None:
    db = CuteDB(db_path)
    db.delete(full_name)
    db.save()


def cute_report(build_report: type,
                path: str,
                name: str | None,
                color: Literal['on', 'off', 'auto']) -> None:
    base: CuteBase | None
    store = CuteStore(JUnitXml.fromfile(path))
    if name is None:
        base = store
    else:
        base = CuteTraversal().find(store, name)
        if base is None:
            raise Exception("test case or suite not found: '{}'".format(name))
    report = build_report()
    base.accept(report)
    CuteConsole(color).show(report)


def cute_result(path: str,
                name: str | None,
                color: Literal['on', 'off', 'auto']) -> None:
    cute_report(CuteResultReport, path, name, color)


def cute_info(path: str,
              name: str | None,
              color: Literal['on', 'off', 'auto']) -> None:
    cute_report(CuteInfoReport, path, name, color)


def main():
    fld_parser = ArgumentParser(add_help = False)
    fld_parser.add_argument('-f',
                            '--fields',
                            type = str,
                            default = None,
                            metavar = 'FIELD_LIST',
                            help = 'Display information fields according to '
                                   'FIELD_LIST, an ordered comma separated '
                                   'list of fields ; use `list\' sub-command '
                                   'to show supported fields')
    clr_parser = ArgumentParser(add_help = False)
    clr_parser.add_argument('-c',
                            '--color',
                            type = str,
                            choices = ['on', 'off', 'auto'],
                            default = 'auto',
                            help = 'Output colorization mode')
    path_parser = ArgumentParser(add_help = False)
    path_parser.add_argument('path',
                              type = str,
                              metavar = 'PATH',
                              help = 'Pathname to XML JUnit file')
    name_parser = ArgumentParser(add_help = False)
    name_parser.add_argument('name',
                             type = str,
                             nargs = '?',
                             default = None,
                             metavar = 'NAME',
                             help = 'Name of test case / suite to show')
    parser = ArgumentParser(description = 'CUTe reporting tool')
    subparser = parser.add_subparsers(dest = 'cmd')
    list_parser = subparser.add_parser('list',
                                       parents = [clr_parser],
                                       help = 'List supported '
                                              'information fields')
    subparser.add_parser('verify',
                         parents = [path_parser],
                         help = 'Exit with overall test result as exit code')
    sum_parser = subparser.add_parser('sumup',
                                      parents = [clr_parser,
                                                 fld_parser,
                                                 path_parser],
                                      help = 'Show a summary of test results')
    sum_parser.add_argument('select',
                            type = str,
                            nargs = '?',
                            default = None,
                            metavar = 'SELECTOR',
                            help = 'Python expression used to select test(s)')
    subparser.add_parser('result',
                         parents = [clr_parser, path_parser, name_parser],
                         help = 'Show result of test given in argument')
    subparser.add_parser('info',
                         parents = [clr_parser, path_parser, name_parser],
                         help = 'Describe test given in argument')
    db_parser = ArgumentParser(add_help = False)
    db_parser.add_argument('db_path',
                            type = str,
                            metavar = 'DBPATH',
                            help = 'Pathname to XML JUnit database file')
    join_parser = subparser.add_parser('join',
                                       parents = [db_parser, path_parser],
                                       help = 'Join JUnit file content into DB')
    join_parser.add_argument('-p',
                             '--parent',
                             type = str,
                             default = None,
                             metavar = 'PARENT',
                             help = 'Full name of parent to join test '
                                    'case / suite to')
    join_parser.add_argument('name',
                             type = str,
                             nargs = '?',
                             default = None,
                             metavar = 'NAME',
                             help = 'Name of test case / suite to insert as')
    del_parser = subparser.add_parser('del',
                                      parents = [db_parser],
                                      help = 'Delete element from JUnit DB')
    del_parser.add_argument('full_name',
                            type = str,
                            metavar = 'FULLNAME',
                            help = 'Full name of test case / suite to remove')
    try:
        args = parser.parse_args()
        if args.cmd == 'list':
            cute_list(args.color)
        elif args.cmd == 'info':
            cute_info(args.path, args.name, args.color)
        elif args.cmd == 'verify':
            exit(cute_verify(args.path))
        elif args.cmd == 'result':
            cute_result(args.path, args.name, args.color)
        elif args.cmd == 'sumup':
            cute_sumup(args.path, args.fields, args.select, args.color)
        elif args.cmd == 'join':
            cute_join(args.db_path, args.path, args.parent, args.name)
        elif args.cmd == 'del':
            cute_delete(args.db_path, args.full_name)
        else:
            raise Exception("'{}': unknown specified command")
    except Exception as e:
        print("{}: {}.".format(arg0, e), file=stderr)
        exit(1)


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
################################################################################
# SPDX-License-Identifier: GPL-3.0-only
#
# This file is part of CUTe.
# Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

from __future__ import annotations
import sys
import enum
import math
import datetime
import junitparser
import rich
import rich.table
import rich.console
import rich.rule
from collections import OrderedDict

class CuteJUnitStatus(enum.Enum):
    PASS = enum.auto()
    SKIP = enum.auto()
    FAIL = enum.auto()
    EXCP = enum.auto()
    OFF = enum.auto()
    NONE = enum.auto()

    def __str__(self):
        return self.name.lower()


class CuteJUnitBase:
    def __init__(self,
                 junit: junitparser.Element,
                 parent: CuteJUnitSuite,
                 status: CuteJUnitStatus,
                 package: str,
                 version: str):
        if not junit.name or len(junit.name) == 0:
            raise Exception("cannot initialize JUnit base element: "
                            "invalid name: '{}'".format(junit.name))
        self._name = junit.name

        self._status = status

        self._elapsed = None
        if self._status != CuteJUnitStatus.OFF:
            if junit.time >= 0:
                secs, usecs = math.modf(junit.time)
                self._elapsed = datetime.timedelta(seconds = secs,
                                                   microseconds = usecs)

        attrs = junit._elem.attrib

        self._timestamp = None
        if self._status != CuteJUnitStatus.OFF:
            if 'timestamp' in attrs.keys():
                try:
                    self._timestamp = datetime.fromisoformat(
                            attrs['timestamp'])
                except Exception:
                    pass

        self._hostname = None
        if self._status != CuteJUnitStatus.OFF:
            if 'hostname' in attrs.keys():
                host = attrs['hostname']
                if host and len(host) > 0:
                    self._hostname = host

        self._package = None
        self._version = None
        if package and len(package) > 0:
            self._package = package
            if version and len(version) > 0:
                self._version = version

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

    def __repr__(self):
        return str(self.__dict__)

    @property
    def parent(self) -> CuteJUnitSuite:
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
    def status(self) -> CuteJUnitStatus:
        return self._status

    @property
    def elapsed(self) -> datetime.timedelta:
        return self._elapsed

    @property
    def timestamp(self) -> datetime.date:
        return self._timestamp

    @property
    def hostname(self) -> str:
        return self._hostname

    @property
    def package_version(self) -> str:
        return self._version

    @property
    def package_name(self) -> str:
        return self._package

    @property
    def package(self) -> str:
        if not self._package:
            return '??'
        if not self._version:
            return self._package
        return self._package + ' ' + self._version

    @property
    def source_path(self) -> str:
        return self._path

    @property
    def source_line(self) -> int:
        return self._line

    @property
    def source(self) -> str:
        if not self._path:
            return '??'
        if self._line:
            line = str(self._line)
        else:
            line = '??'
        return '{}:{}'.format(self._path, line)


class CuteJUnitCase(CuteJUnitBase):
    def __init__(self, junit: junitparser.TestCase, parent: CuteJUnitSuite):
        attrs = junit._elem.attrib

        status = '??'
        if 'status' in attrs.keys():
            s = attrs['status']
            if s and len(s) > 0:
                status = s
        if status != 'disabled':
            if junit.result:
                res = junit.result[0]
                if isinstance(res, junitparser.Skipped):
                    status = CuteJUnitStatus.SKIP
                elif isinstance(res, junitparser.Failure):
                    status = CuteJUnitStatus.FAIL
                elif isinstance(res, junitparser.Error):
                    status = CuteJUnitStatus.EXCP
                else:
                    status = CuteJUnitStatus.NONE
            else:
                    status = CuteJUnitStatus.PASS
        else:
            status = CuteJUnitStatus.OFF

        super().__init__(junit,
                         parent,
                         status,
                         parent.package_name,
                         parent.package_version)

    def children(self) -> Iterator[CuteJUnitBase]:
        # Return an empty generator...
        yield from ()

    def accept(self, visit: CuteJUnitVisitor) -> None:
        visit.case(self)


class CuteJUnitSuite(CuteJUnitBase):
    def __init__(self,
                 junit: junitparser.TestSuite,
                 parent: CuteJUnitSuite):
        self._children = []
        self._count = junit.tests

        attrs = junit._elem.attrib

        self._off = 0
        if 'disabled' in attrs.keys():
            self._off = int(attrs['disabled'])

        if self._off != self._count:
            self._exec = self._count - self._off
            self._skip = junit.skipped
            self._fail = junit.failures
            self._excp = junit.errors
            self._pass = self._exec - self._skip - self._fail - self._excp
            if self._skip == self._exec:
                status = CuteJUnitStatus.SKIP
            elif (self._pass + self._skip) == self._exec:
                status = CuteJUnitStatus.PASS
            else:
                status = CuteJUnitStatus.FAIL
        else:
            self._exec = 0
            self._skip = 0
            self._fail = 0
            self._excp = 0
            self._pass = 0
            status = CuteJUnitStatus.OFF

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

        super().__init__(junit, parent, status, package, version)

    @property
    def test_count(self) -> int:
        return self._count

    @property
    def exec_count(self) -> int:
        return self._exec

    @property
    def off_count(self) -> int:
        return self._off

    @property
    def pass_count(self) -> int:
        return self._pass

    @property
    def skip_count(self) -> int:
        return self._skip

    @property
    def fail_count(self) -> int:
        return self._fail

    @property
    def excp_count(self) -> int:
        return self._excp

    def register(self, base: CuteJUnitBase) -> None:
        self._children.append(base)

    def children(self) -> Iterator[CuteJUnitBase]:
        for child in self._children:
            yield child

    def accept(self, visit: CuteJUnitVisitor) -> None:
        visit.suite(self)


class CuteJUnitStore:
    def __init__(self, junit: junitparser.JUnitXml):
        self._suites = []
        if isinstance(junit, junitparser.TestSuite):
            self._suites.append(self._load_junit_suite(junit, None))
        elif isinstance(root, JUnitXml):
            for suite in junit:
                self._suites.append(self._load_junit_suite(suite, None))

        self._elapsed = datetime.timedelta(0)
        self._count = 0
        self._exec = 0
        self._off = 0
        self._pass = 0
        self._skip = 0
        self._fail = 0
        self._excp = 0
        for suite in self._suites:
            self._elapsed += suite.elapsed
            self._count += suite.test_count
            self._exec += suite.exec_count
            self._off += suite.off_count
            self._pass += suite.pass_count
            self._skip += suite.skip_count
            self._fail += suite.fail_count
            self._excp += suite.excp_count

        if self._off != self._count:
            if self._skip == self._exec:
                self._status = CuteJUnitStatus.SKIP
            elif (self._pass + self._skip) == self._exec:
                self._status = CuteJUnitStatus.PASS
            else:
                self._status = CuteJUnitStatus.FAIL
        else:
            self._status = CuteJUnitStatus.OFF

    def _load_junit_suite(self,
                          junit: junitparser.Element,
                          parent: junitparser.Suite) -> CuteJUnitSuite:
        root = CuteJUnitSuite(junit, parent)
        for suite in junit.testsuites():
            self._load_junit_suite(suite, root)
        for case in junit.iterchildren(junitparser.TestCase):
            CuteJUnitCase(case, root)
        return root

    def suites(self) -> Iterator[CuteJUnitSuite]:
        for suite in self._suites:
            yield suite

    def accept(self, visit: CuteJUnitVisitor) -> None:
        visit.store(self)

    @property
    def status(self) -> CuteJUnitStatus:
        return self._status

    @property
    def elapsed(self) -> datetime.timedelta:
        return self._elapsed

    @property
    def test_count(self) -> int:
        return self._count

    @property
    def exec_count(self) -> int:
        return self._exec

    @property
    def off_count(self) -> int:
        return self._off

    @property
    def pass_count(self) -> int:
        return self._pass

    @property
    def skip_count(self) -> int:
        return self._skip

    @property
    def fail_count(self) -> int:
        return self._fail

    @property
    def excp_count(self) -> int:
        return self._excp


class CuteJUnitConsole(rich.console.Console):
    def __init__(self, tty: Optional[bool] = None):
        super().__init__(force_terminal = tty)

    def show(self, renderable: rich.console.ConsoleRenderable) -> None:
        self.print(renderable)


class CuteJUnitNameField:
    STYLE = rich.style.Style(color = 'blue')

    @staticmethod
    def column(store: CuteJUnitStore) -> rich.table.Column:
        return rich.table.Column(header = 'NAME',
                                 footer = 'Total',
                                 justify = 'left',
                                 no_wrap = False)

    @staticmethod
    def render_suite(suite: CuteJUnitSuite) -> rich.console.RenderableType:
        _guide = ('+-- ', '|   ', '`-- ')
        guide = ''
        depth = suite.depth
        if depth:
            while depth > 1:
                guide += _guide[1]
                depth -= 1
            guide += _guide[0]
        return rich.text.Text(guide + suite.name + '\n' + _guide[1],
                              style = CuteJUnitNameField.STYLE)


    @staticmethod
    def render_case(case: CuteJUnitCase) -> str:
        _guide = ('+-- ', '|   ', '`-- ')
        guide = ''
        depth = case.depth
        if depth:
            while depth > 1:
                guide += _guide[1]
                depth -= 1
            guide += _guide[0]
        return guide + case.name



class CuteJUnitStatusField:
    PASS_STYLE = rich.style.Style(color = 'green')
    SKIP_STYLE = rich.style.Style(color = 'yellow')
    FAIL_STYLE = rich.style.Style(color = 'red')
    EXCP_STYLE = rich.style.Style(color = 'red')
    OFF_STYLE = rich.style.Style(color = 'bright_black', bold = True)

    @staticmethod
    def column(store: CuteJUnitStore) -> rich.table.Column:
        foot = rich.text.Text(str(store.status),
                              style = CuteJUnitStatusField.style(store.status))
        return rich.table.Column('STAT',
                                 footer = foot,
                                 justify = 'center',
                                 no_wrap = True)

    @staticmethod
    def render_suite(suite: CuteJUnitSuite) -> rich.console.RenderableType:
        return rich.text.Text(str(suite.status) + '\n',
                              style = CuteJUnitStatusField.style(suite.status))

    @staticmethod
    def render_case(case: CuteJUnitCase) -> str:
        return rich.text.Text(str(case.status),
                              style = CuteJUnitStatusField.style(case.status))

    @staticmethod
    def style(status: CuteJunitstatus) -> rich.style.Style:
        if status == CuteJUnitStatus.PASS:
            return CuteJUnitStatusField.PASS_STYLE
        elif status == CuteJUnitStatus.SKIP:
            return CuteJUnitStatusField.SKIP_STYLE
        elif status == CuteJUnitStatus.FAIL or \
             status == CuteJUnitStatus.EXCP:
            return CuteJUnitStatusField.FAIL_STYLE
        elif status == CuteJUnitStatus.OFF:
            return CuteJUnitStatusField.OFF_STYLE
        else:
            raise Exception("field style not found: "
                            "unsupported status: '{}'".format(str(status)))


class CuteJUnitElapsedField:
    @staticmethod
    def column(store: CuteJUnitStore) -> rich.table.Column:
        return rich.table.Column('TIME',
                                 footer = str(store.elapsed),
                                 justify = 'right',
                                 no_wrap = True)

    @staticmethod
    def render_suite(suite: CuteJUnitSuite) -> str:
        return CuteJUnitElapsedField._render(suite) + '\n'

    @staticmethod
    def render_case(case: CuteJUnitCase) -> str:
        return CuteJUnitElapsedField._render(case)

    @staticmethod
    def _render(base: CuteJUnitBase) -> str:
        if not base.elapsed:
            return ''
        return str(base.elapsed)


class CuteJUnitCountField:
    @staticmethod
    def column(label: str,
               count: int,
               total: int,
               style: rich.style.Style) -> rich.table.Column:
        foot = rich.text.Text(str(count) +
                              '\n' +
                              CuteJUnitCountField._percent(count, total),
                              style = style)
        return rich.table.Column(label,
                                 footer = foot,
                                 justify = 'right',
                                 no_wrap = True)

    def render_case(self, case: CuteJUnitCase) -> str:
        return ''

    @staticmethod
    def _percent(part: int, total: int) -> str:
        if total == 0:
                return '0%'
        return '{}%'.format(round(part * 100 / total))


class CuteJUnitPassField(CuteJUnitCountField):
    STYLE = CuteJUnitStatusField.PASS_STYLE

    @staticmethod
    def column(store: CuteJUnitStore) -> rich.table.Column:
        return CuteJUnitCountField.column('#PASS',
                                          store.pass_count,
                                          store.exec_count,
                                          CuteJUnitPassField.STYLE)

    @staticmethod
    def render_suite(suite: CuteJUnitSuite) -> str:
        return str(suite.pass_count) + \
               '\n' + \
               CuteJUnitCountField._percent(suite.pass_count, suite.exec_count)


class CuteJUnitSkipField(CuteJUnitCountField):
    STYLE = CuteJUnitStatusField.SKIP_STYLE

    @staticmethod
    def column(store: CuteJUnitStore) -> rich.table.Column:
        return CuteJUnitCountField.column('#SKIP',
                                          store.skip_count,
                                          store.exec_count,
                                          CuteJUnitSkipField.STYLE)

    @staticmethod
    def render_suite(suite: CuteJUnitSuite) -> str:
        return str(suite.skip_count) + \
               '\n' + \
               CuteJUnitCountField._percent(suite.skip_count, suite.exec_count)


class CuteJUnitFailField(CuteJUnitCountField):
    STYLE = CuteJUnitStatusField.FAIL_STYLE

    @staticmethod
    def column(store: CuteJUnitStore) -> rich.table.Column:
        return CuteJUnitCountField.column('#FAIL',
                                          store.fail_count,
                                          store.exec_count,
                                          CuteJUnitFailField.STYLE)

    @staticmethod
    def render_suite(suite: CuteJUnitSuite) -> str:
        return str(suite.fail_count) + \
               '\n' + \
               CuteJUnitCountField._percent(suite.fail_count, suite.exec_count)


class CuteJUnitExcpField(CuteJUnitCountField):
    STYLE = CuteJUnitStatusField.EXCP_STYLE

    @staticmethod
    def column(store: CuteJUnitStore) -> rich.table.Column:
        return CuteJUnitCountField.column('#EXCP',
                                          store.excp_count,
                                          store.exec_count,
                                          CuteJUnitExcpField.STYLE)

    @staticmethod
    def render_suite(suite: CuteJUnitSuite) -> str:
        return str(suite.excp_count) + \
               '\n' + \
               CuteJUnitCountField._percent(suite.excp_count, suite.exec_count)


class CuteJUnitExecField(CuteJUnitCountField):
    STYLE = None

    @staticmethod
    def column(store: CuteJUnitStore) -> rich.table.Column:
        return CuteJUnitCountField.column('#EXEC',
                                          store.exec_count,
                                          store.test_count,
                                          CuteJUnitExecField.STYLE)

    @staticmethod
    def render_suite(suite: CuteJUnitSuite) -> str:
        return str(suite.exec_count) + \
               '\n' + \
               CuteJUnitCountField._percent(suite.exec_count, suite.test_count)


class CuteJUnitTotalField(CuteJUnitCountField):
    STYLE = None

    @staticmethod
    def column(store: CuteJUnitStore) -> rich.table.Column:
        foot = rich.text.Text(str(store.test_count),
                              style = CuteJUnitTotalField.STYLE)
        return rich.table.Column('#TOTAL',
                                 footer = foot,
                                 justify = 'right',
                                 no_wrap = True)

    @staticmethod
    def render_suite(suite: CuteJUnitSuite) -> str:
        return str(suite.test_count) + '\n'


class CuteJUnitVisitor:
    def suite(self, suite: CuteJUnitSuite) -> None:
        pass

    def case(self, case: CuteJUnitCase) -> None:
        pass


class CuteJUnitReport(CuteJUnitVisitor):
    _bold_style = rich.style.Style(bold = True)
    _box = rich.box.Box('    \n'  # top line
                        '    \n'  # header row
                        '    \n'  # line underneath header low
                        '    \n'  # data rows
                        '----\n'  # line separating data row sections
                        '----\n'  # line above footer row
                        '    \n'  # footer row
                        '    \n', # bottom line
                        ascii = True)
    _fields = OrderedDict({
            'name':  CuteJUnitNameField(),
            'pass':  CuteJUnitPassField(),
            'skip':  CuteJUnitSkipField(),
            'fail':  CuteJUnitFailField(),
            'excp':  CuteJUnitExcpField(),
            'exec':  CuteJUnitExecField(),
            'total': CuteJUnitTotalField(),
            'time':  CuteJUnitElapsedField(),
            'stat':  CuteJUnitStatusField()
    })

    def __init__(self, fields: Optional[list[str]] = None):
        if fields:
            if len(fields) == 0:
                raise Exception('cannot init report: '
                                'empty specified field list')
            visible = tuple(fields)
            for f in visible:
                if f not in self._fields:
                    raise Exception("cannot init report: "
                                    "unknown specified field: '{}'".format(f))
        else:
            visible = list(self._fields)
        self._visible = visible

    @staticmethod
    def fields() -> list[str]:
        return list(CuteJUnitReport._fields)

    def store(self, store: CuteJUnitStore) -> None:
        columns = tuple(f.column(store) for f in self._fields.values())
        self._table = rich.table.Table(*columns,
                                       title = self._title(store),
                                       caption = self._caption(store),
                                       box = self._box,
                                       show_edge = False,
                                       pad_edge = False,
                                       show_header = True,
                                       show_footer = True)

    def suite(self, suite: CuteJUnitSuite):
        cols = tuple(self._fields[f].render_suite(suite) for f in self._visible)
        self._table.add_row(*cols)

    def case(self, case: CuteJUnitCase):
        cols = tuple(self._fields[f].render_case(case) for f in self._visible)
        self._table.add_row(*cols)

    def _title(self, store: CuteJUnitStore) -> rich.console.RenderableType:
        text = rich.text.Text('Test results', style = self._bold_style)
        rule = rich.rule.Rule(title = text,
                              characters = '#',
                              align = 'center',
                              style = None)
        return rich.padding.Padding(rule, pad = (0, 0, 1, 0))

    def _caption(self, store: CuteJUnitStore) -> rich.console.RenderableType:
        if store.status == CuteJUnitStatus.PASS:
            status = 'All tests PASSED'
        elif store.status == CuteJUnitStatus.SKIP:
            status = 'All tests SKIPPED'
        elif store.status == CuteJUnitStatus.FAIL or \
             store.status == CuteJUnitStatus.EXCP:
            cnt = store.fail_count + store.excp_count
            status = '{}/{} ({}%) tests FAILED'.format(cnt,
                                                       store.exec_count,
                                                       round(cnt * 100 /
                                                             store.exec_cnt))
        elif store.status == CuteJUnitStatus.OFF:
            status = 'All tests DISABLED'
        else:
            raise Exception("cannot setup report: "
                            "invalid status: '{}'".format(store.status))

        status_text = rich.text.Text(style = None)
        status_text.append('### ')
        status_text.append(status,
                           style = CuteJUnitStatusField.style(store.status) +
                                   self._bold_style)
        status_text.append(' ###')

        title = 'SUMMARY'
        pad = len(status) - len(title)
        left = int(pad / 2)
        right = pad - left
        title_text = rich.text.Text(style = None)
        title_text.append('### ')
        title_text.append('{:<{}}{}{:>{}}'.format('', left, title, '', right)),
        title_text.append(' ###')
        return rich.padding.Padding(rich.console.Group(title_text, status_text),
                                    pad = (1, 0, 0, 0))

    def show(self, console: CuteJUnitConsole) -> None:
        console.show(rich.padding.Padding(self._table, pad = (1, 0, 0, 0)))


def cute_build_report(store: CuteJUnitStore, report: CuteJUnitReport):
    def walk(base: CuteJUnitBase, report: CuteJUnitReport):
        #if isinstance(base, CuteJUnitCase):
        #    return
        base.accept(report)
        for child in base.children():
            walk(child, report)

    store.accept(report)
    for suite in store.suites():
        walk(suite, report)


def cute_sumup(store: CuteJUnitStore) -> None:
    report = CuteJUnitReport()
    cute_build_report(store, report)
    console = CuteJUnitConsole()
    report.show(console)


xml = junitparser.JUnitXml.fromfile(sys.argv[1])
store = CuteJUnitStore(xml)
#store.visit(CuteJUnitDebugVisitor())
#print(store.elapsed)
cute_sumup(store)

#_suite = junitparser.TestSuite('une suite')
#_case = junitparser.TestCase('un test', time = 0.1)
#_suite.add_testcase(_case)
#_autre = junitparser.TestCase('un autre test', time = 0.5)
#_suite.add_testcase(_autre)
#
#suite = CuteJUnitSuite(_suite, None)
#case = CuteJUnitCase(_case, suite)
#autre = CuteJUnitCase(_autre, suite)
#print(suite.full_name)
#print(case.full_name)
#print(autre.full_name)
#
#for b in suite.children():
#    print(b.depth, ':', b.name, b.elapsed)
#
##case = CuteJUnitCase(junit, None)


"""
import sys
import os
import argparse
from junitparser import JUnitXml, TestCase, TestSuite, Failure, Skipped, Error
from prettytable import PrettyTable, HEADER, NONE
from datetime import datetime
from collections import OrderedDict
from termcolor import colored

class CuteFormatter:
    def __init__(self, mode: str):
        if mode == 'on':
            self._isatty = True
        elif mode == 'off':
            self._isatty = False
        elif mode == 'auto':
            self._isatty = sys.stdout.isatty()
        else:
            raise Exception("unsupported color mode: '{}'".format(mode))

    def status(self, status: str) -> str:
        if status == 'pass':
            return colored(status, 'green') if self._isatty else status
        elif status == 'skip':
            return colored(status, 'yellow') if self._isatty else status
        elif status == 'fail':
            return colored(status, 'red') if self._isatty else status
        elif status == 'excp':
            return colored(status, 'red') if self._isatty else status
        elif status == 'off':
            return colored(status, 'grey', attrs=['bold']) \
                    if self._isatty else status
        else:
            raise Exception("unsupported status: '{}'".format(status))

    def header(self, title: str) -> str:
        return colored(title, attrs=['bold']) if self._isatty else title

    def suite(self, name: str) -> str:
        return colored(name, 'blue') if self._isatty else name


def status_field(item):
    stat = '??'
    attrs = item._elem.attrib
    if 'status' in attrs.keys():
        s = attrs['status']
        if s and len(s) > 0:
            stat = s
    return stat


def disabled_count(suite):
    attrs = suite._elem.attrib
    if 'disabled' in attrs.keys():
        return int(attrs['disabled'])
    else:
        return 0


def host_string(item):
    host = '??'
    attrs = item._elem.attrib
    if 'hostname' in attrs.keys():
        h = attrs['hostname']
        if h and len(h) > 0:
            host = h
    return host


def package_string(item):
    pkg = '??'
    vers = ''
    attrs = item._elem.attrib
    if 'package' in attrs.keys():
        p = attrs['package']
        if p and len(p) > 0:
            pkg = p
    if 'version' in attrs.keys():
        v = attrs['version']
        if v and len(v) > 0:
            vers = ' ' + v
    return pkg + vers


def source_string(item):
    path = '??'
    line = '??'
    attrs = item._elem.attrib
    if 'file' in attrs.keys():
        p = attrs['file']
        if p and len(p) > 0:
            path = p
    if 'line' in attrs.keys():
        l = attrs['line']
        if l and len(l) > 0:
            line = str(l)
    return '{}:{}'.format(path, line)


def timestamp_string(item):
    attrs = item._elem.attrib
    if 'timestamp' in attrs.keys():
        try:
            return datetime.fromisoformat(attrs['timestamp']).strftime('%c')
        except Exception:
            pass
    return '??'


class TermVisitor:
    _fields = OrderedDict({
        'stat': ('STAT',        'c', 'status of test case / suite'),
        'name': ('NAME',        'l', 'name of test case / suite'),
        'pass': ('#PASS',       'r', 'number of passed test cases'),
        'skip': ('#SKIP',       'r', 'number of skipped test cases'),
        'fail': ('#FAIL',       'r', 'number of failed test cases'),
        'excp': ('#EXCP',       'r', 'number of crashed test cases'),
        'exec': ('#EXEC',       'r', 'number of executed test cases'),
        'test': ('#TEST',       'r', 'total number of test cases'),
        'type': ('TYPE',        'c', 'either \'suite\' or \'case\''),
        'pkg':  ('PKG',         'c', 'package name'),
        'time': ('TIME (s.us)', 'r', 'elapsed running time expressed as <seconds>.<microseconds>'),
        'date': ('DATE',        'c', 'run time starting date expressed in the local time zone'),
        'host': ('HOST',        'c', 'name of host that ran test cases / suites'),
        'src':  ('SRC',         'l', 'source file pathname and line number of test case / suite')
    })

    def __init__(self, formatter: CuteFormatter, fields: list[str]):
        if fields:
            if len(fields) == 0:
                raise Exception("empty specified field list")
            visible = tuple(fields)
            for f in visible:
                if f not in self._fields:
                    raise Exception("'{}': unknown specified field".format(f))
        else:
            visible = list(self._fields)
        self._visible = visible
        self._format = formatter
        self._table = PrettyTable([self._format.header(self._fields[f][0])
                                   for f in self._visible],
                                  hrules = HEADER,
                                  vrules = NONE,
                                  valign = 'm',
                                  float_format = '.6')
        for f in self._visible:
            title = self._format.header(self._fields[f][0])
            align = self._fields[f][1]
            self._table.align[title] = align
        self._package = None
        self._host = None

    @staticmethod
    def fields():
        return list(TermVisitor._fields)

    @staticmethod
    def fields_desc(formatter: CuteFormatter) -> str:
        labels = (formatter.header('FIELDS'), formatter.header('DESCRIPTION'))
        table = PrettyTable(labels,
                            hrules = HEADER,
                            vrules = NONE,
                            valign = 'm')
        table.align[labels[0]] = 'c'
        table.align[labels[1]] = 'l'
        for f, t in TermVisitor._fields.items():
            table.add_row([f, t[2]])
        return table.get_string()

    def render_text(self, **kwargs):
        return self._table.get_string(**kwargs)

    def _add_row(self,
                 status,
                 name,
                 passed,
                 skipped,
                 failures,
                 errors,
                 execs,
                 tests,
                 kind,
                 package,
                 time,
                 tstamp,
                 host,
                 source):
        row = []
        for f in self._visible:
            if f == 'stat':
                row.append(status)
            elif f == 'name':
                row.append(name)
            elif f == 'pass':
                row.append(passed)
            elif f == 'skip':
                row.append(skipped)
            elif f == 'fail':
                row.append(failures)
            elif f == 'excp':
                row.append(errors)
            elif f == 'exec':
                row.append(execs)
            elif f == 'test':
                row.append(tests)
            elif f == 'type':
                row.append(kind)
            elif f == 'pkg':
                row.append(package)
            elif f == 'time':
                row.append(time)
            elif f == 'date':
                row.append(tstamp)
            elif f == 'host':
                row.append(host)
            elif f == 'src':
                row.append(source)
            else:
                raise Exception("'{}': unexpected field", f)
        self._table.add_row(row)

    def suite(self, suite, depth):
        name = '{:<{}}{}'.format('', depth * 3, suite.name)
        disabled = disabled_count(suite)
        self._package = package_string(suite)
        source = source_string(suite)

        if disabled != suite.tests:
            execs = suite.tests - disabled
            passed = execs - suite.skipped - suite.failures - suite.errors
            if suite.skipped == execs:
                stat = 'skip'
            elif (passed + suite.skipped) == execs:
                stat = 'pass'
            else:
                stat = 'fail'
            skipped = suite.skipped
            failures = suite.failures
            errors = suite.errors
            tstamp = timestamp_string(suite)
            self._host = host_string(suite)
        else:
            stat = 'off'
            passed = 0
            skipped = 0
            failures = 0
            errors = 0
            execs = 0
            tstamp = ''
            self._host = ''
        self._add_row(self._format.status(stat),
                      self._format.suite(name),
                      passed,
                      skipped,
                      failures,
                      errors,
                      execs,
                      suite.tests,
                      'suite',
                      self._package,
                      suite.time,
                      tstamp,
                      self._host,
                      source)

    def case(self, case, depth):
        name = '{:<{}}{}'.format('', depth * 3, case.name)
        source = source_string(case)

        if status_field(case) != 'disabled':
            if case.result:
                res = case.result[0]
                if isinstance(res, Skipped):
                    stat = 'skip'
                elif isinstance(res, Failure):
                    stat = 'fail'
                elif isinstance(res, Error):
                    stat = 'excp'
                else:
                    stat = '??'
            else:
                    stat = 'pass'
            tstamp = timestamp_string(case)
            host = self._host
        else:
            stat = 'off'
            tstamp = ''
            host = ''
        self._add_row(self._format.status(stat),
                      name,
                      '',
                      '',
                      '',
                      '',
                      '',
                      '',
                      'case',
                      self._package,
                      case.time,
                      tstamp,
                      host,
                      source)


def walk_suite(root, visitor, depth):
    visitor.suite(root, depth)
    for suite in root.testsuites():
        walk_suite(suite, visitor, depth + 1)
    for case in root.iterchildren(TestCase):
        visitor.case(case, depth + 1)


def walk_junit(root, visitor):
    if isinstance(root, TestSuite):
        walk_suite(root, visitor, 0)
    elif isinstance(root, JUnitXml):
        for elm in root:
            walk_suite(elm, visitor, 0)


def junit_has_failed(xml: JUnitXml) -> bool:
    return (xml.failures + xml.errors) != 0


def junit_sumup(xml: JUnitXml, fields: list[str]) -> str:
    return 'implement me'


def junit_show(xml, formatter, fields, select):
    visit = TermVisitor(formatter, fields)
    walk_junit(xml, visit)
    return visit.render_text()


def junit_list_fields(formatter: CuteFormatter):
    return TermVisitor.fields_desc(formatter)


def main():
    global arg0

    arg0 = os.path.basename(sys.argv[0])

    fld_parser = argparse.ArgumentParser(add_help = False)
    fld_parser.add_argument('-f',
                            '--fields',
                            type = str,
                            nargs = 1,
                            default = None,
                            metavar = 'FIELD_LIST',
                            help = 'Display information fields according to '
                                   'FIELD_LIST, an ordered comma separated '
                                   'list of fields ; use `list\' sub-command '
                                   'to list supported fields')
    clr_parser = argparse.ArgumentParser(add_help = False)
    clr_parser.add_argument('-c',
                            '--color',
                            type = str,
                            choices = ['on', 'off', 'auto'],
                            nargs = 1,
                            default = ['auto'],
                            help = 'Output colorization mode')
    path_parser = argparse.ArgumentParser(add_help = False)
    path_parser.add_argument('path',
                              type = str,
                              nargs = 1,
                              default = None,
                              metavar = 'JUNIT_PATH',
                              help = 'Pathname to XML JUnit file')
    parser = argparse.ArgumentParser(description = 'CUTe XML JUnit tool')
    subparser = parser.add_subparsers(dest = 'cmd')
    list_parser = subparser.add_parser('list',
                                       parents = [clr_parser],
                                       help = 'List supported fields')
    res_parser = subparser.add_parser('result',
                                      parents = [path_parser],
                                      help = 'Return with result of an XML JUnit file as exit code')
    sum_parser = subparser.add_parser('sum',
                                      parents = [clr_parser, fld_parser, path_parser],
                                      help = 'Sumup content of an XML JUnit file')
    show_parser = subparser.add_parser('show',
                                       parents = [clr_parser, fld_parser, path_parser],
                                       help = 'Show content of an XML JUnit file')
    args = parser.parse_args()
    cmd = args.cmd

    try:
        if args.cmd == 'list':
            print(junit_list_fields(CuteFormatter(args.color[0])))
            sys.exit(0)

        xml = JUnitXml.fromfile(args.path[0])

        if args.cmd == 'result':
            sys.exit(junit_has_failed(xml))

        all_fields = TermVisitor.fields()
        if args.fields is not None:
            fields = args.fields[0].split(',')
            for f in fields:
                if f not in all_fields:
                    print("{}: invalid specified field: '{}'.".format(arg0, f),
                          file = sys.stderr)
                    sys.exit(1)
        else:
            fields = [ 'stat',
                       'name',
                       'pass',
                       'skip',
                       'fail',
                       'excp',
                       'exec',
                       'test',
                       'time' ]

        if args.cmd == 'sum':
            print(junit_sumup(xml, fields))
        elif args.cmd == 'show':
            print(junit_show(xml, CuteFormatter(args.color[0]), fields, None))
        else:
            raise Exception("'{}': unknown specified command")
    except Exception as e:
        raise e
        print("{}: {}.".format(arg0, e), file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
"""

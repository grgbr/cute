Welcome to Stroll documentation
###############################

.. Caption of toctrees are not translated into latex, hence the dirty trick
.. below. See https://github.com/sphinx-doc/sphinx/issues/3169 for more infos.
.. Basically, we ask the latex backend to generate a \part{} section for each
.. toctree caption using the `raw' restructuredtext directive.

.. only:: latex

   .. raw:: latex

      \part{User Guide}

.. toctree::
   :maxdepth: 2
   :numbered:
   :caption: User Guide

   user


.. only:: latex

   .. raw:: latex

      \part{Integration Guide}

.. toctree::
   :numbered:
   :caption: Integration Guide

   install


.. We use the latex_appendices setting into conf.py to benefit from native latex
.. appendices section numbering scheme. As a consequence, there is no need to
.. generate appendix entries for latex since already requested through the
.. latex_appendices setting.

.. only:: latex

   .. raw:: latex

      \part{Appendix}

.. only:: html

   .. toctree::
      :caption: Appendix

      todo
      genindex

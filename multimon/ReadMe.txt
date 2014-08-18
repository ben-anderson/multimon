========================================================================
    Multimon: A multi monitor tool

  Licenced under the GPL V2. Copyright(c) 2014 Ben Anderson.
========================================================================


Multimon is a multi monitor tool which watches for newly created top level
windows and checks which monitor they were created on.

If they were not created on the same monitor that the mouse is on, they will
be moved to that monitor.

Windows are sized to fit the new monitor.


----------
TODO LIST
----------

- Create a nice icon.

- Add a control panel to allow per application window position overrides.

- Consolidate log.cpp and logserver.cpp.

- Move log.cpp and mmap.cpp into a library.


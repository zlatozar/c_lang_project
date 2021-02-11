## Spacemacs key bindings

### Help

Key&nbsp;Binding   | Description
---------   | --------------
`C-h`       | _(meta)_ Various options
`M-m ?`     | Search key bindings
`M-m F1`    | Search command definitions
`M-m h d`   | _(meta)_ Descriptions
`M-m h d m` | Describe current mode

### Emacs

Key&nbsp;Binding   | Description
---------   | --------------
`M-f`, `M-b` | Word foreword/backward
`C-n`, `C-p` | Next/Previous line
`C-x C-t`    | Transpose lines
`C-s`, `C-r` | Search foreword/backward
`C-g g`      | Go to line
`C-x (`      | Start macro
`C-x )`      | End macro

#### Windows

Key&nbsp;Binding   | Description
---------   | --------------
`M-m w m`   | Toggle many to one windows
`Alt-1, 2..9 | Jump window

#### Jumps

Key&nbsp;Binding   | Description
---------   | --------------
`M-m j l`   | Jump to line
`M-m j w`   | Jump to word
`M-m j D`   | Jump to file directory

#### Rings

Key&nbsp;Binding   | Description
---------   | --------------
`M-m r`     | _(meta)_ Rings
`M-m r y`   | Show killing ring

#### Files

Key&nbsp;Binding   | Description
---------   | --------------
`M-m f f`   | Find file relative from current dir
`M-m f E`   | **sudo** edit
`M-m f j`   | Jump from dir
`M-m f L`   | Search file everywhere
`M-m f r`   | Show recent files
`M-m f t`   | Toggle `treemax` explorer
`M-m q s`   | Save all files and exit

### Projects

Key&nbsp;Binding   | Description
---------   | --------------
`M-m p p`   | List/Import projects
`M-m /`     | Search project using `ag`
`M-m *`     | Search _current_ word in project
`M-m p f`   | Find project file
`M-m p b`   | Show open project buffers
`M-m p !`   | Open shell in project root dir
`M-m p t`   | Toggle project `treemax`
`M-m p &`   | Async shell command run (relative to project root)
`M-m p k`   | Kill all project buffers
`M-m p r`   | Recent project files
`M-m p R`   | Project search and replace
`M-m s a P` | Search project with `ag`

### Development

Key&nbsp;Binding   | Description
---------   | --------------
`C-c`       | _(meta)_ Various options
`M-m c`     | _(meta)_ Compilation
`M-m c c`   | Compile project
`M-m g`     | _(meta) Git commands
`M-m g s`   | Git status
`M-m a u`   | Visual undo tree
`M-m e`     | _(meta)_ Errors
`M-m e l`   | Toggle errors list
`M-m e .`   | Browse errors
`C-c C-s`   | Start REPL
`M-m j (`   | Check parents
`M-m j c`   | Go to last change
`M-m j i`   | Jump to function
`M-m s s`   | Helm `swoop`
`M-m s o`   | Overlay symbol
`M-m p a`   | Switch between impl/test
`M-m j 0`   | Add mark
`M-m r m`   | Show all marks
`M-m j j`   | `avy-timer` (type and go)
`M-m x w d` | Meaning of the word at point
`M-m ;`     | Comment region

### Misc

Key&nbsp;Binding   | Description
---------   | --------------
`M-m m`     | Describe current major mode
`M-m S a`   | Add word at point in dictionary
`M-m C-v m` | Rectangle mark

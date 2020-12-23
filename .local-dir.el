;; Tell 'clangd' where to find 'compile_commands.json'
((c++-mode . ((lsp-clients-clangd-args . ("--compile-commands-dir=build"
                                          "--pch-storage=memory"
                                          "--background-index"
                                          "-j=4"))
              )))

;; Debugging. Do not forget 'M-x dap-gdb-lldb-setup'
((c++-mode . ((dap-debug-template-configurations . (("my_app"
                                                     :type "gdb"
                                                     :target "/home/user/src/build/app/my_app"))
                                                 ))))

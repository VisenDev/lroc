;; Codegen latest test
;; February 11 2025
;; This file is written after both codegen.fnl and cgen.fnl




;;internal
(var indent-level 1)
(local out io.write)
(fn indent []
  (for [i 1 indent-level 10]
    (out "    ")))
(local primitives
  {:int "int"
   :uint "unsigned int"
   :float "float"
   :double "double"
   :char "char"
   :bool "int"
   })
(fn validate-type [type]
  




;; top level 
(macro constant [name type value]



;; test code
(constant entity-cap :int 1024)
(variable entity-len :uint 0)
(structure entity 
  [x :float]
  [y :float]
  [health :int]
  )

(function main :int [[argc :int] [argv :char**]]
  (constant times :int 10)
  (variable i :int 0)
  (while (less-than i times)
    (printf "hello")
    (assign i (plus i 1)) 
    )
  (return 0))

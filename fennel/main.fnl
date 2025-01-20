(fn cconst [ctype name value]
  (io.write :const " " ctype " " name " = " value ";\n"))

(fn cvar [ctype name value]
  (io.write ctype " " name " = " value ";\n"))

(cconst :int "i" 1)
(cvar :int "j" 0)

(macro test [symbol]
  `(print ,(string symbol)))

(test hi)

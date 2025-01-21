(macro struct [name fields]
  (var result "")
  (each [_ field (ipairs fields)]
    (set result (.. result "    "))
      (each [_ ident (ipairs field)]
        (set result
             (.. result " " (tostring ident))))
    (set result (.. result ";\n")))
  `(print (.. "struct " ,(tostring name) "\n{\n" ,result "}")))

(macro c-assign [sexpr]
  (assert-repl (= (. sexpr 1) `assign))
  `(print "assigned")
  )


(macro c-block [...]
  (var result {})
  (each [index sexpr (ipairs ...)]
    (when (= (. sexpr 1) `printf)
      (tset result index `(print "printf")))
    )
  result
  )

(macro function [return-type name params ...]
  (var param-string "")
  (each [index param (ipairs params)]
    (when (not (= index 1)) (set param-string (.. param-string ",")))
      (each [_ tok (ipairs param)]
        (set param-string (.. param-string " " (tostring tok))))
      )
  `(do
     (print (..
       ,(tostring return-type) " "
       ,(tostring name) "("
       ,param-string ") {\n"))
     (c-block (,...))
     (print "\n}\n")
     ))


(struct vector2 ((float x) (float y)))
(struct vector3
  ((float x)
   (float y)
   (float z)))



(function int main ((int argc) (char ** argv))
          (printf "hi")
          (printf "hi")
          (printf "hello")
          )


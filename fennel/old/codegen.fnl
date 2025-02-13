(macro c-raw [indent & rest]
  (assert-compile (= (type indent) "number")
                  "c-raw indentation level must be a number")
  (each [i elem (ipairs rest)]
    (case (type elem)
      "string" (tset rest i elem)
      _ (tset rest i (tostring elem))
       ))
    `(do
       (for [_# 1 ,(- indent 1) 1]
           (io.write "    "))
       (io.write (.. (table.unpack ,rest)))
       ))

(macro c-struct [name fields]
  (var result "")
  (each [_ field (ipairs fields)]
    (set result (.. result "    "))
      (each [_ ident (ipairs field)]
        (set result
             (.. result " " (tostring ident))))
    (set result (.. result ";\n")))
  `(c-raw 1 "struct " ,name "\n{\n" ,result "}\n\n"))

(macro c-assign [name value]
  `(do
     (c-raw 1 ,name " = ")
     (c-expr ,value)
     (c-raw 1 ";\n")
  ))

(macro c-expr [sexpr]
    (fn exclude-first [tbl]
      (var result {})
      (each [index elem (ipairs tbl)]
        (when (not (= index 1))
          (tset result (- index 1) elem)))
      result)
  (case (tostring (. sexpr 1))
    _ `(c-raw
         1 ;indent
         ,(. sexpr 1) ;print function name
         "(" ;print open parens
         ,(table.unpack (exclude-first sexpr))
          ");\n" ;print end parens
          )))


(macro c-block [indent & rest]
    (fn exclude-first [tbl]
      (var result {})
      (each [index elem (ipairs tbl)]
        (when (not (= index 1))
          (tset result (- index 1) elem)))
      result)
  (each [index sexpr (ipairs rest)]
    (local block-indent (+ indent 1))
    (tset rest index
      (case (tostring (. sexpr 1))
        "c-assign" `(do (c-raw ,block-indent) ,sexpr)
        "c-block"  sexpr
        _ `(c-raw
             ,(+ 1 indent) ;indent
             ,(. sexpr 1) ;print function name
             "(" ;print open parens
             ,(table.unpack (exclude-first sexpr))
              ");\n" ;print end parens
              )
  )))
  `(do (c-raw ,indent "{\n")
     ,rest
     (c-raw ,indent "}\n")
     ))


;      (tset result index `(c-assign ,sexpr)))
;      (= (. sexpr 1) `
;    )
;  result

(macro c-function [return-type name params ...]
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
       ,param-string ")"))
     (c-block 1 ,...)
     ))


(c-struct vector2 ((float x) (float y)))
(c-struct vector3
  ((float x)
   (float y)
   (float z)))



(c-function int main ((int argc) (char ** argv))
  (printf "hi")
  (printf "hi")
  (printf "hello")
  (c-assign asdf (+ 1 2))
  (c-block 2
    (printf "hello world")
    (printf "hello john")
    )
)


(macro struct [name fields]
  (var result "")
  (each [_ field (ipairs fields)]
    (set result (.. result "    "))
      (each [_ ident (ipairs field)]
        (set result
             (.. result " " (tostring ident))))
    (set result (.. result ";\n")))
  `(print (.. "struct " ,(tostring name) "\n{\n" ,result "}")))

(macro function [return-type name params ...]
  `(print (.. ,(tostring return-type) " " ,(tostring name))))

(struct vector2 ((float x) (float y)))
(struct vector3
  ((float x)
   (float y)
   (float z)))



(function int main (void) 
          (printf "hi"))


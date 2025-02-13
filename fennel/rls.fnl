;;Utils
(macro incr [num]
  `(set ,num (+ ,num 1)))
(macro decr [num]
  `(set ,num (- ,num 1)))
(lambda identifier? [str] (string.match str "^[a-zA-Z_][a-zA-Z0-9_]*$"))
(lambda member? [value list]
  (var found false)
  (each [_ val (ipairs list)]
    (when (= val value)
      (set found true)))
  found)

;;Internals
(var cgen {})
(set cgen {
     :indent-level 1
     :out io.write
     :indentation (lambda []
                    (for [i 1 cgen.indent-level 1]
                      (cgen.out "    ")))
     :indent (lambda [] (incr cgen.indent-level))
     :unindent (lambda [] (decr cgen.indent-level))
     })
(var sema {
  :identifiers [{}]
  :bound?
    (lambda [self identifier]
           (not (= nil (?. self.identifiers identifier))))
  :unbound? 
    (lambda [self identifier]
           (not (self:bound? identifier)))
  :is-type?
    (lambda [self type-id]
      (and (self:bound? type-id)
           (member? (?. (?. self.identifiers type-id) :category)
                    [:type-primitive :type-enum :type-struct :type-union])
           ))
  :record-primitive
    (lambda [self type-id c-representation]
      (assert (self:unbound? type-id)
              (.. "Multiply defined symbol \"" type-id "\""))
      (assert (identifier? type-id)
        (.. "Invalid primitive name \"" type-id "\""))
      (tset self.identifiers type-id {
            :c-representation c-representation
            :category :type-primitive})
      )})
  :record-enum 
    (lambda [self type-id fields]
      (assert (self:unbound? type-id)
              (.. "Multiply defined symbol \"" type-id "\""))
      (assert (identifier? type-id)
              (.. "Invalid enum name \"" type-id "\""))
      (each [_ value (ipairs fields)]
            (assert (identifier? value)
                    (.. "Invalid enum field name \"" value "\"")))
      (tset self.identifiers type-id {
            :category :type-enum
            :c-representation type-id
            :fields :fields
            }
            ))
  :record-struct
    (lambda [self type-id fields]
      (assert (self:unbound? type-id)
              (.. "Multiply defined symbol \"" type-id "\""))
      (assert (identifier? type-id)
              (.. "Invalid enum name \"" type-id "\""))
      (each [_ field (ipairs fields)]
            (assert (identifier? field.type-id)
                    (.. "Invalid struct field type \"" field.type-id "\""))
            (assert (identifier? field.name)
                    (.. "Invalid struct field name \"" field.name "\""))
            )
      (tset self.identifiers type-id {
            :category :type-enum
            :c-representation type-id
            :fields :fields
            }
            ))
            

(sema:record-primitive :int "int")
(sema:record-primitive :uint "unsigned int")

(cgen.indent)
(cgen.indentation)
(cgen.out "hi")
(cgen.unindent)

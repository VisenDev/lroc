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

(lambda last [list]
  (. list (length list)))

(lambda push [list value]
  (tset list (+ 1 (length list))
        value))
(lambda pop [list]
  (table.remove list (length list)))

;;Internals
(var cgen {
     :indent-level 1
     :out io.write
     :indentation (lambda [self]
                    (for [i 1 self.indent-level 1]
                      (self.out "    ")))
     :indent (lambda [self] (incr self.indent-level))
     :unindent (lambda [self] (decr self.indent-level))
     })

(var sema {
     :identifiers [{}]
     })

(set sema.bound?
     (lambda [self identifier]
       (var found false)
       (each [_ scope (ipairs self.identifiers)]
         (when
           (not (= nil (?. self.identifiers identifier)))
           (set found true)
           ))
       found))

(set sema.unbound? 
     (lambda [self identifier]
       (not (self:bound? identifier))))

(set sema.block-start
     (lambda [self]
     "Enters a new scope for identifiers"
       (push self.identifiers {})))

(set sema.block-end
     (lambda [self]
     "Exits a scope for identifiers"
       (pop self.identifiers)))

(set sema.bind 
     (lambda [self identifier value]
       (assert (self:unbound? identifier)
               (.. "Multiply defined symbol \"" identifier "\""))
       (tset (last self.identifiers)
             identifier value)))

(set sema.is-type?
     (lambda [self type-id]
       (and (self:bound? type-id)
            (member? (?. (?. self.identifiers type-id) :category)
                     [:type-primitive :type-enum :type-struct :type-union])
            )))
(set sema.record-primitive
     (lambda [self type-id c-representation]
       (assert (self:unbound? type-id)
               (.. "Multiply defined symbol \"" type-id "\""))
       (assert (identifier? type-id)
               (.. "Invalid primitive name \"" type-id "\""))
       (self:bind type-id {
             :c-representation c-representation
             :category :type-primitive})
       ))
(set sema.record-enum 
     (lambda [self type-id fields]
       (assert (self:unbound? type-id)
               (.. "Multiply defined symbol \"" type-id "\""))
       (assert (identifier? type-id)
               (.. "Invalid enum name \"" type-id "\""))
       (each [_ value (ipairs fields)]
         (assert (identifier? value)
                 (.. "Invalid enum field name \"" value "\"")))
       (self:bind type-id {
             :category :type-enum
             :c-representation type-id
             :fields :fields
           })))
(set sema.record-struct
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
       (self:bind type-id {
             :category :type-enum
             :c-representation type-id
             :fields :fields
             }
             )))


(sema:record-primitive :int "int")
(sema:record-primitive :uint "unsigned int")

(cgen:indent)
(cgen:indentation)
(cgen.out "hi")
(cgen:unindent)

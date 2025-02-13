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
(assert (member? 1 [1 2 3 4]) "member? failed")
(assert (member? 4 [1 2 3 4]) "member? failed")

(macro orelse [maybe-nil backup-value]
  `(if (= nil ,maybe-nil) ,backup-value ,maybe-nil))

(macro last [list]
  `(. ,list (length ,list)))

(macro push! [list value]
  `(tset ,list (+ 1 (length ,list))
        ,value))
(macro pop! [list]
  `(table.remove ,list (length ,list)))

(lambda dump-table [tbl ?indentation]
  (io.write "{\n")
  (each [key value (pairs tbl)]
    (io.write (string.rep "  " (orelse ?indentation 1)))
    (if (= (type value) :table)
        (do
          (io.write key ": ")
          (dump-table value (+ 1 (orelse ?indentation 1)))
          )
        (do
          (io.write key)
          (io.write ": ")
          (io.write (.. "\"" (tostring value) "\""))
          (io.write "\n"))))
    (io.write (string.rep "  " (- (orelse ?indentation 0) 1)))
    (io.write "}\n")
    )

(macro assert-not-null [value]
  `(assert (not (= nil ,value))
           "Value is null"))


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

(lambda sema.bound? [self identifier]
  (var found false)
  (each [_ scope (ipairs self.identifiers)]
    (when
      (not (= nil (?. scope identifier)))
      (set found true)
      ))
  found)

(lambda sema.unbound? [self identifier]
  (not (self:bound? identifier)))

(lambda sema.block-start [self]
  "Enters a new scope for identifiers"
  (push! self.identifiers {}))

(lambda sema.block-end [self]
  "Exits a scope for identifiers"
  (pop! self.identifiers))

(lambda sema.bind [self identifier value]
  (assert (self:unbound? identifier)
          (.. "Multiply defined symbol \"" identifier "\""))
  (assert (identifier? identifier)
          (.. "Invalid identifier name \"" identifier "\""))
  (tset (last self.identifiers) identifier value)
  (assert (self:bound? identifier)
          (.. "Internal error, failed to bind symbol \"" identifier "\""))
  )

(lambda sema.type? [self type-id]
  "returns whether the type-id is a valid defined type"
  (assert-not-null self)
  (assert-not-null type-id)
  (assert-not-null self.identifiers)
  (and (self:bound? type-id)
       (do 
         (var found false)
         (each [_ scope (ipairs self.identifiers)]
           (assert-not-null scope)
           (local identifier-value (?. scope type-id))
           (local category (?. identifier-value :category))
           (assert-not-null category)
           (when (and 
                   (not (= nil category))
                   (member? category
                            [:type-primitive :type-enum :type-struct :type-union]))
             (set found true)))
         found)
       ))

(lambda sema.record-primitive [self type-id c-representation]
  (assert (self:unbound? type-id)
          (.. "Multiply defined symbol \"" type-id "\""))
  (assert (identifier? type-id)
          (.. "Invalid primitive name \"" type-id "\""))
  (self:bind type-id {
             :c-representation c-representation
             :category :type-primitive})
  )

(lambda sema.record-enum [self type-id fields]
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
             :fields fields
             }))

(lambda sema.record-struct [self type-id fields]
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
             :category :type-struct
             :c-representation type-id
             :fields fields
             }
             ))


(sema:record-primitive :int "int")
(assert (sema:type? :int))

(sema:record-primitive :uint "unsigned int")
(assert (sema:type? :uint))

(sema:record-primitive :float "float")
(assert (sema:type? :float))

(sema:record-enum :error_codes [:none :out_of_memory :out_of_bounds])
(assert (sema:type? :error_codes))

(sema:record-struct :vector2 [{:name :x :type-id :float} {:name :y :type-id :float}])
(assert (sema:type? :vector2))

(io.write "\nsema: ")
(dump-table sema)


;(cgen:indent)
;(cgen:indentation)
;(cgen.out "hi")
;(cgen:unindent)

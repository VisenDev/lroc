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
  ;(local indentation 
  ;       (if (= ?indentation nil) 0 ?indentation))
  ;(io.write (string.rep "  " indentation))
  ;(io.write "{\n")
  ;(io.write (string.rep "  " (+ 1 indentation)))

  ;(io.write (string.rep "  " (orelse ?indentation 0)))
  (io.write "{\n")
  (each [key value (pairs tbl)]
    (io.write (string.rep "  " (orelse ?indentation 1)))
    ;(print "VALUE" value)
    (if (= (type value) :table)
        (do
          (io.write key ": ")
          (dump-table value (+ 1 (orelse ?indentation 1)))
          )
        (do
          (io.write key)
          (io.write ": ")
          (io.write (tostring value))
          (io.write "\n"))))
    (io.write (string.rep "  " (- (orelse ?indentation 0) 1)))
    (io.write "}\n")
    )

  ;(io.write (string.rep "  " indentation))
  ;(io.write "}\n")))

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
           (not (= nil (?. scope identifier)))
           (set found true)
           ))
       found))

(set sema.unbound? 
     (lambda [self identifier]
       (not (self:bound? identifier))))

(set sema.block-start
     (lambda [self]
     "Enters a new scope for identifiers"
       (push! self.identifiers {})))

(set sema.block-end
     (lambda [self]
     "Exits a scope for identifiers"
       (pop! self.identifiers)))

(set sema.bind 
     (lambda [self identifier value]
       (assert (self:unbound? identifier)
               (.. "Multiply defined symbol \"" identifier "\""))
       (assert (identifier? identifier)
               (.. "Invalid identifier name \"" identifier "\""))
       (tset (last self.identifiers) identifier value)
       (assert (self:bound? identifier)
               (.. "Internal error, failed to bind symbol \"" identifier "\""))
       ))

(set sema.type?
     (lambda [self type-id]
       "returns whether the type-id is a valid defined type"
       (and (self:bound? type-id)
            (do 
              (var found false)
              (each [_ scope (ipairs self.identifiers)]
                (local identifier-value (?. self.identifiers type-id))
                (local category (?. identifier-value :category))
                (print (.. "category is " (tostring category)))
                (when (and 
                        (not (= nil category))
                        (member? category
                              [:type-primitive :type-enum :type-struct :type-union]))
                  (set found true)))
              found)
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
;(print (sema:bound? :int))
;(print (sema:type? :int))
(dump-table sema)
(print " ")
(dump-table {:hello :world :hi :there :foo {:bar :bap :nil :nap}})


;(cgen:indent)
;(cgen:indentation)
;(cgen.out "hi")
;(cgen:unindent)

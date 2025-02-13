;; Codegen latest test
;; February 11 2025
;; This file is written after both codegen.fnl and cgen.fnl

;;utils
(lambda table-contains-value? [tbl value]
  (var result false)
  (each [key val (pairs tbl)]
    (when (= val value)
      (set result true)))
  result)

(lambda table-contains-key? [tbl key]
  (not (= nil (?. tbl key))))


;;internal
(var compiler {
  :indent-level 1
  :types {}
  })

(local out io.write)
(lambda indent-out [] 
  (for [i 1 compiler.indent-level 10]
    (out "    ")))
(lambda indent [] (set compiler.indent-level (+ 1 compiler.indent-level)))
(lambda unindent [] (set compiler.indent-level (- compiler.indent-level 1)))

(lambda validate-identifier [name]
  (local modified (string.gsub name "-" "_"))
  (when (not (string.match modified "^[a-zA-Z_][a-zA-Z0-9_]*$"))
    (error (.. "invalid identifier -> \"" modified "\"" )))
  modified)


(lambda validate-type [type]
  (local basetype (string.gsub type "*" ""))
  (when (not (table-contains-key? compiler.types type))
    (error (.. "undefined type -> \"" type "\"")))
  type)

(lambda make-primitive [name c-representation]
  "Record primitive type information into the compiler"
  (local typename (validate-identifier name))
  (assert (= false (table-contains-key? compiler.types typename))
               (.. "Type \"" name "\" is already defined!"))
  (tset compiler.types typename
        {:category :primitive :c c-representation}))


;;; Built In Primitive Types
(make-primitive :int "int")
(make-primitive :uint "unsigned int")
(make-primitive :char "char")
(make-primitive :ichar "signed char")
(make-primitive :uchar "unsigned char")
(make-primitive :float "float")
(make-primitive :double "double")
(make-primitive :long "long")
(make-primitive :ulong "unsigned long")
(make-primitive :short "short")
(make-primitive :ushort "unsigned short")
(make-primitive :bool "int")

(lambda make-enum [name fields] 
  "Record enum type information into the compiler"
  (local enumname (validate-identifier name))
  (assert (= name enumname)
          "type names should be validated before being passed to make functions")
  (assert (= false (table-contains-key? compiler.types enumname))
               (.. "Type \"" name "\" is already defined!"))
  (var enumtype {
       :category :enum
       :fields {}
       :c enumname
       })
  (each [key value (ipairs fields)]
    (local fieldname (validate-identifier  value))
    (assert (= false (table-contains-value? enumtype.fields fieldname))
               (.. "Field \"" fieldname "\" of enum \"" enumname "\" is already defined!"))
    (tset enumtype.fields key fieldname))
  (tset compiler.types enumname enumtype))

;; c interface functionality
(macro enum [name fields]
  "Define a new enum"
  (local field (gensym))
  (local name-str (gensym))
  (local fields-str (gensym))
  (local _ (gensym))
  (local value (gensym))
  `(do
    (local ,name-str (validate-identifier ,(tostring name)))
    (local ,fields-str
           (icollect [,_ ,value
                        (ipairs
                         ,(icollect [_ val (ipairs fields)]
                                    (tostring val)))
                        ]
             (validate-identifier ,value)))
    (make-enum ,name-str ,fields-str)
    (out "typedef enum {\n")
    (indent)
    (each [,_ ,field (ipairs ,fields-str)]
      (indent-out)
      (out (.. ,name-str "_" ,field ",\n")))
    (unindent)
    (out "} ")
    (out ,name-str)
    (out ";\n")
    ))

(enum test [a b c])
(enum error [nil out-of-memory index-out-of-bounds])

;;(lambda validate-type [type]
;;  (local basetype (string.gsub type "*" ""))
;;  (local typevalue (?. compiler.types basetype))
;;  (when (= typevalue nil)
;;    (error (.. "invalid type -> " type)))
;;  type)
;;
;;(lambda validate-identifier [name]
;;  (when (not (string.match name "^[a-zA-Z_][a-zA-Z0-9_]*$"))
;;    (error (.. "invalid identifier -> " name)))
;;  name)
;;
;;(lambda valid-type-category? [category] 
;;  "returns whether the type category is a valid type category"
;;  (local valid-categories 
;;         [:int :float :bool :double :uint :struct :enum :union :char])
;;  (each [_ value (ipairs valid-categories)]
;;    (when (= value category)
;;      (return true)))
;;  (return false))
;;
;;(assert-table-has-field [tbl field]
;;  (when (= nil (?. tbl field))
;;    (error (.. "table missing field " (tostring tbl) (tostring field)))))

;;(valid-type-metadata? [metadata]
;;  (when (= nil (?. tbl :out))
;;
;;(lambda register-new-type [name category metadata] 
;;  (when (not (= nil (?. compiler.types name)))
;;    (error (.. "multiply defined type name -> " name)))
;;  (assert (valid-type-category? category))
;;  (tset compiler (validate-name name) {: category : metadata}))

;; public api functions
;;(macro struct [name fields-table]



  


;;
;;;; top level 
;;(macro constant [name type value]
;;
;;
;;
;;;; test code
;;(constant entity-cap :int 1024)
;;(variable entity-len :uint 0)
;;(structure entity 
;;  [x :float]
;;  [y :float]
;;  [health :int]
;;  )
;;
;;(function main :int [[argc :int] [argv :char**]]
;;  (constant times :int 10)
;;  (variable i :int 0)
;;  (while (less-than i times)
;;    (printf "hello")
;;    (assign i (plus i 1)) 
;;    )
;;  (return 0))

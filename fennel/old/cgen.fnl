(local out io.write)

(local valid-types {
   :int "int"
   :uint "unsigned int"
   :float "float"
   :double "double"
   :char "char"
   :bool "int"
   })

(fn validate-type [typename] 
  (local typestring (?. valid-types typename))
  (when (= typestring nil) 
    (error (.. "invalid type -> " typename)))
  typestring)

(fn validate-name [name] 
  (when (not (string.match name "^[a-zA-Z_][a-zA-Z0-9_]*$"))
    (error (.. "invalid identifier -> " name)))
  name
  )


(fn constant [typename name value]
  (out "const ")
  (out (validate-type typename))
  (out " ")
  (out (validate-name name))
  (out " = ")
  (out value)
  (out ";\n")
  )

(fn structure [name fields]
  (out "typedef struct {\n")
  (each [key value (pairs fields)]
    (out "    ")
    (out (validate-type key))
    (out " ")
    (out (validate-name value))
    (out ";\n"))
  (out "} ")
  (out (validate-name name))
  (out ";\n"))

(fn enumeration [name fields]
  (out "typedef enum {\n")
  (var first true);
  (each [_ value (ipairs fields)]
    (when (not first)
      (out ",\n"))
    (out "    ")
    (out (validate-name value))
    (set first false)
    )
  (out "\n} ")
  (out (validate-name name))
  (out ";\n"))


(macro function [returntype name arguments ...] 
  (local first (gensym))
  (local key (gensym))
  (local value (gensym))
 `(do
    (out (validate-type ,returntype))
    (out " ")
    (out (validate-name ,name))
    (out "(")
    (var ,first true)
    (each [,key ,value (ipairs ,arguments)]
      (when (not ,first)
        (out ", "))
      (out (validate-type ,key))
      (out " ")
      (out (validate-name ,value))
      (set ,first false))
    (out ") {\n")
    ,...
    (out "}\n")
    ))





;; Test Code
(constant :bool :hello 1)
(constant :int :max 10)
(constant :uint :min 0)

(structure :vector2 {
  :int :x
  :int :y
    })

(structure :vector3 {
  :float :x
  :float :y
    })

(enumeration :errors
  [:error_none :error_out_of_memory :error_out_of_bounds])


(function :int :main {:int :argc :int :argv :uint :aasdf :bool :hi}
  (constant :uint :times 1)
  )


;; Alternate interface

(constant entity-cap :int 1024)
(variable entity-len :uint 0)
(structure entity 
  [x :float]
  [y :float]
  [health :int]
  )

(function main :int [[argc :int] [argv :char-ptr-ptr]]
  (constant times :int 10)
  (variable i :int 0)
  (while (less-than i times)
    (printf "hello"))
  (return 0))

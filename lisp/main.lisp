
;;; IO Utilities
(defparameter *indentation* 0)
(defun out (str)
  (format t "~a" str))
(defun newline ()
  (format t "~%"))
(defun indentation ()
  (loop :for i :from 0 :below *indentation*
        :do (format t "    ")))
(defun indent ()
  (incf *indentation*))
(defun unindent ()
  (decf *indentation*))


;;; C Keywords
(defparameter *keywords*
'("auto" "break" "case" "char" "const" "continue" "default" "do"
    "double" "else" "enum" "extern" "float" "for" "goto" "if"
    "inline" "int" "long" "register" "restrict" "return" "short"
    "signed" "sizeof" "static" "struct" "switch" "typedef" "union"
    "unsigned" "void" "volatile" "while" "_Alignas" "_Alignof"
    "_Atomic" "_Bool" "bool" "_Complex" "_Generic" "_Imaginary"
    "_Noreturn" "_Static_assert" "static_assert" "_Thread_local"))


;;; General utilities

(defun keyword-p (str)
  (member str *keywords*))

(defun alpha-digit-p (ch)
  (or (alpha-char-p ch)
      (digit-char-p ch)))

(defun alpha-digit-underscore-p (ch)
  (or (alpha-digit-p ch)
      (char= ch #\_)))

(defun alpha-underscore-p (ch)
  (or (alpha-char-p ch)
      (char= ch #\_)))

(defun identifier-p (str) 
  (and
    (alpha-underscore-p (nth 0 (coerce str 'list)))
    (every #'alpha-digit-underscore-p (coerce str 'list))
    ;(not (keyword-p str))
    ))

(define-symbol-macro no-default
                (error "initialization value must be provided"))

;;; Compiler State
(deftype identifier () `(satisfies identifier-p))

;;; Type Representation Structures

#|
(defstruct c-primitive 
  "c-typename is a string not an identifier because 
  c primitive types can be multiple words, as in unsigned int"
  (c-typename no-default :type string)
  )
(defstruct c-enum
  (c-typename no-default :type identifier)
  (c-fields no-default :type list)
  )
(defstruct c-field
  "struct or union field representation"
  (c-typename no-default :type identifier)
  (c-name no-default :type identifier)
  )
(defstruct c-struct
  (c-typename no-default :type identifier)
  (c-fields no-default :type list)
  )
(defstruct c-union 
  (c-typename no-default :type identifier)
  (c-fields no-default :type list)
  )
|#

(defclass c-primitive ()
  ((c-typename :initarg :c-typename
               :accessor c-typename
               :type string
  )))

(defclass c-enum ()
  ((c-typename :initarg :c-typename
               :accessor c-typename
               :type symbol)
   (c-fields :initarg :c-fields
             :accessor c-fields
             :type list)))

(defclass c-field ()
  ((c-typename :initarg :c-typename
               :accessor c-typename
               :type symbol)
   (c-name :initarg :c-name
           :accessor c-name
           :type symbol)))

(defclass c-struct ()
  ((c-typename :initarg :c-typename
               :accessor c-typename
               :type symbol)
   (c-fields :initarg :c-fields
             :accessor c-fields
             :type list)))

(defclass c-union ()
  ((c-typename :initarg :c-typename
               :accessor c-typename
               :type symbol)
   (c-fields :initarg :c-fields
             :accessor c-fields
             :type list)))




;;; Possible Identifier Values
(deftype identifier-type-specification ()
    `(or ,@'(c-primitive c-enum c-struct c-union)))
  
(defstruct identifier-c-variable
  (is-const t :type boolean)
  (c-typename no-default :type identifier)
  )

(defstruct identifier-c-function
  (c-name no-default :type identifier)
  (return-typename no-default :type identifier)
  (args nil :type list))

(deftype identifier-value ()
  `(or ,@'(identifier-type-specification identifier-c-variable identifier-c-function)))

;;; More Compiler State
(defparameter *identifiers-stack* (list (make-hash-table :test 'equalp)))
(defun get-identifier (name)
  (loop :for tbl :in *identifiers-stack*
        :do (when (not (equalp nil (gethash name tbl)))
              (return-from get-identifier (gethash name tbl))))
  nil)
(defun set-identifier (name value)
  (when (not (equalp nil (get-identifier name)))
    (error "symbol ~a already defined" name))
  (setf (gethash name (car (last *identifiers-stack*))) value))


;;; Builtin identifiers
(set-identifier "int" (make-instance 'c-primitive :c-typename "int"))
(set-identifier "uint" (make-instance 'c-primitive :c-typename "unsigned int"))



(defun def-internal (name type value)
  (when (not (equalp nil (get-identifier name)))
    (error "symbol ~a already defined" name))
  (set-identifier name (make-identifier-c-variable :c-typename type
                                                   :is-const t))
  (indentation)
  (out "const ")
  (out (c-typename (the identifier-type-specification (get-identifier type))))
  (out " ")
  (out name)
  (out " = ")
  (out value)
  (out ";")
  (newline)
  )

(defun variable-internal (name type value)
  (when (not (equalp nil (get-identifier name)))
    (error "symbol ~a already defined" name))
  (set-identifier name (make-identifier-c-variable :c-typename type
                                                   :is-const nil))
  (indentation)
  (out (c-typename (the identifier-type-specification (get-identifier type))))
  (out " ")
  (out name)
  (out " = ")
  (out value)
  (out ";")
  (newline)
  )

(defun set-internal (name value)
  (let*
    ((variable-info (the identifier-c-variable (get-identifier name)))
     )
    (assert (null
            (identifier-c-variable-is-const variable-info))
        (name) "attempt to set constant variable \"~a\"" name)
    (indentation)
    (out name)
    (out " = ")
    (out value)
    (out ";")
    (newline)
    ))
  

(def-internal "hello" "int" 1)
(variable-internal "foo" "int" 1)
(set-internal "foo" 2)


;;; Public Api Code
;(defpackage :c)
;(in-package :c)





;;; Test/Example Usage Code


(indentation)
(out "int main ()")
(newline)
(indent)
(indentation)
(out "int main ()")
(unindent)
(newline)


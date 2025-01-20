;;;Utility functions
(defun name-of (symbol)
  (string-downcase (symbol-name symbol)))

(defun concat (&rest strings)
  (apply #'concatenate 'string strings))

  

;;;Interface functions
(defun c-include (file) 
  (format nil "#include \"~a\"~%" file))

(defun c-define (name value) 
  (format nil "#define ~a ~a ~%" name value))

(defun c-typedef (type name)
  (format nil "typedef ~a ~a;~%" type name))

(define-symbol-macro c-int "int")
(define-symbol-macro c-uint "unsigned int")
(define-symbol-macro c-long "long")
(define-symbol-macro c-ulong "unsigned long")
(define-symbol-macro c-float "float")
(define-symbol-macro c-double "double")
(define-symbol-macro c-bool "int")
(define-symbol-macro c-char "char")

(defmacro c-enum (name fields)
  `(c-typedef (concat
            "enum {"
            ,(reduce (lambda (a b) (concat a ", " b))
                   fields)
            "}")
           ,name))
  
#|
(defun c-enum (name fields)
  (let ((result ""))
    (dolist (field fields)
      (setf result (concatenate 'string result
                                (concatenate 'string name "_" field)
                                ", " )))
    (format nil "enum ~a {~a}" name result)))

(defun struct (name fields)
  (dolist (field fields)
    ()))

;(struct "vector2" ("int" "x") ("int" "y"))
|#

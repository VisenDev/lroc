;;utilies
(defmacro define (&rest rest) 
  `(comment ,(format nil "~{ ~a~}" rest) :prefix "#define"))

(defun symbol-concat (a b)
  (intern (concatenate 'string (symbol-name a) (symbol-name b))))

(defmacro define-vec (base-type) 
  (symbol-macrolet ((name (symbol-concat base-type '_vec))
                    (base-type-ptr base-type*)
                    )
   `(progn

      (struct ,name
        (decl (
          (,base-type *items)
          (long len)
          (long cap))))
      (typedef struct ,name ,name)

      (function ,(symbol-concat name '_init) () -> ,name
        (decl ((,name result)
               )
          (set result.items (malloc (* (sizeof ,base-type) 8)))
          (assert result.items)
          (set result.len 0)
          (set result.cap 0)
          (return result)))

      (function ,(symbol-concat name '_free) ((,name *self)) -> void
        (free self->items))

      (function ,(symbol-concat name '_get)
          ((,name *self)(long index)) -> ,base-type-ptr
        (when (or (< index 0) (> index self->len))
          (return NULL))
        (return &self->items[index]))

      (function ,(symbol-concat name '_increase_capacity)
          ((,name *self)(long new_cap)) -> int
        (set self->items (realloc self->items (* new_cap (sizeof ,base-type))))
        (when (== self->items NULL)
          (return 1))
        (set self->cap new_cap)
        (return 0)
        )

      (function ,(symbol-concat name '_set)
          ((,name *self)(long index)(,base-type value)) -> int
        (when (or (< index 0) (>= index self->len))
          (return 1))
        (set self->items[index] value)
        (return 0))

      (function ,(symbol-concat name '_append)
          ((,name *self)(,base-type value)) -> int
        (when (>= self->len (1- self->cap))
          (,(symbol-concat name '_increase_capacity)
            self (* 2 (+ self->cap 1))))
        (set self->len (1+ self->len))
        (return (,(symbol-concat name '_set) self (1- self->len) value))
        )


      )))


(defmacro defer (statement &rest body)
  `(progn
     ,@body
     ,statement
     ))

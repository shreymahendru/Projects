#| Assignment 1 - Functional Shakespeare Interpreter

Read through the starter code carefully. In particular, look for:

- interpret: the main function used to drive the program.
  This is provided for you, and should not be changed.
- evaluate: this is the main function you'll need to change.
  Please put all helper functions you write below this one.
  Doing so will greatly help TAs when they are marking. :)
|#
#lang racket

; You are allowed to use all the string functions in this module.
; You may *not* import any other modules for this assignment.
(require racket/string)

; This exports the main driver function. Used for testing purposes.
; This is the only function you should export. Don't change this line!
(provide interpret)

;------------------------------------------------------------------------------
; Parsing constants
;------------------------------------------------------------------------------

; Sections dividers
(define personae "Dramatis personae")
(define settings "Settings")
(define finis "Finis")

; Comment lines
(define comments '("Act" "Scene"))

; List of all "bad words" in a definition
(define bad-words
  '("vile"
    "villainous"
    "wicked"
    "naughty"
    "blackhearted"
    "shameless"
    "scoundrelous"))

; Arithmetic
(define add "join'd with")
(define mult "entranc'd by")

; Self-reference keywords
(define self-refs
  '("I"
    "me"
    "Me"
    "myself"
    "Myself"))

; Function call
(define call "The song of")

; Function parameter name
(define param "Hamlet")

;------------------------------------------------------------------------------
; Interpreter driver
;------------------------------------------------------------------------------

#|
(interpret filename)
  filename: a string representing the path to a FunShake file

  Returns a list of numbers produced when evaluating the FunShake file.
  You can complete this assignment without modifying this function at all,
  but you may change the implementation if you like. Please note that you may
  not change the interface, as this is the function that will be autotested.
|#
(define (interpret filename)
  (let* ([contents (port->string (open-input-file filename))]
         [lines (map normalize-line (string-split contents "\n"))]
         ; Ignore title, empty, and comment lines
         [body (remove-empty-and-comments (rest lines))])
    (display body)))

#|
(normalize-line str)
  str: the line string to normalize

  Remove trailing period and whitespace.
|#
(define (normalize-line str)
  (string-trim (string-normalize-spaces (string-trim str)) "."))

#|
(remove-empty-and-comments strings)
  strings: a list of strings

  Removes all empty strings and FunShake comment strings from 'strings'.
|#
(define (remove-empty-and-comments strings)
  (filter (lambda (s)
            (and
             (< 0 (string-length s))
             (not (ormap (lambda (comment) (prefix? comment s))
                         comments))))
          strings))

#|
(prefix? s1 s2)
  s1, s2: strings

  Returns whether 's1' is a prefix of 's2'.
|#
(define (prefix? s1 s2)
  (and (<= (string-length s1) (string-length s2))
       (equal? s1 (substring s2 0 (string-length s1)))))

;------------------------------------------------------------------------------
; Main evaluation (YOUR WORK GOES HERE)
;------------------------------------------------------------------------------

#|
(evaluate body)
  body: a list of lines corresponding to the semantically meaningful text
  of a FunShake file.

  Returns a list of numbers produced when evaluating the FunShake file.
  This should be the main starting point of your work! Currently,
  it just outputs the semantically meaningful lines in the file.
|#
(define (evaluate body)
  (let* ([descriptions (format-desc(rest (first (section-split body))))]
        [setting (if (equal? (first (first (rest (section-split body)))) settings)
                      (build-settings(format-settings(rest (first (rest (section-split body))))))
                      "empty")]
        [dialogue (evaluate-dialogue (format-dialogue (last (section-split body))) descriptions setting 0)])            
    (map (lambda (val)
           (second val))
         dialogue))
         
  )

;split the sections, returning a nested list with each section being a sublist.
(define (section-split lst)
  (if (equal? (member finis lst) #f) (list lst)
  (cons (before-index lst (sublist '("Finis") lst))
        (section-split (member (list-ref lst (+ (sublist '("Finis") lst) 1)) lst)))))

;format descriptions. i.e. (list (list <id> <value>) (list <id> <value>) ...)
(define (format-desc exp-list)
  (map (lambda (line)
         (map (lambda (x)
                (if (equal? (length (string-split x)) 1) x
                    (eval (string-split x))))                 
              (regexp-split #px", " line)))
       exp-list))

;format settings into proper format. i.e. (list (list <id> <exp>) (list <id> <exp>) ...)
(define (format-settings exp-list)
  (map (lambda (line)
         (regexp-split #px", " line))
       exp-list))

;build settings functions returns (list (list <id> <procedure>) (list <id> <procedure) ...)
(define (build-settings exp-list)
  (map (lambda (line)
         (map (lambda (x)
                (cond [(equal? (length (string-split x)) 1) x]

                      
                      ;addition
                      [(sublist (string-split add) (string-split x))

                       
                       (if (member "Hamlet" (string-split x))
                           ;has parameter (Hamlet)
                           (lambda (param)
                             (+ param
                                (+ (if (equal? (eval (before-index (string-split x) (sublist (string-split add) (string-split x)))) 1) 0
                                       (eval (before-index (string-split x) (sublist (string-split add) (string-split x)))))
                                   (if (equal? (eval (member (list-ref (string-split x) (+ 2 (sublist (string-split add) (string-split x)))) (string-split x))) 1) 0
                                       (eval (slice (string-split x) (+ 2 (sublist (string-split add) (string-split x))) )))))
                             )
                           ;no param
                           (lambda (param)
                             (+ (eval (before-index (string-split x) (sublist (string-split add) (string-split x))))
                                (eval (slice (string-split x) (+ 2 (sublist (string-split add) (string-split x))) ))))
                           )]

                      ;mult
                      [(sublist (string-split mult) (string-split x))

                       
                       (if (member "Hamlet" (string-split x))
                           ;has parameter (Hamlet)
                           (lambda (param)
                             (* param
                                (+ (if (equal? (eval (before-index (string-split x) (sublist (string-split mult) (string-split x)))) 1) 0
                                       (eval (before-index (string-split x) (sublist (string-split mult) (string-split x)))))
                                   (if (equal? (eval (member (list-ref (string-split x) (+ 2 (sublist (string-split mult) (string-split x)))) (string-split x))) 1) 0
                                       (eval (slice (string-split x) (+ 2 (sublist (string-split mult) (string-split x))) )))))
                             )
                           ;no param
                           (lambda (param)
                             (* (eval (before-index (string-split x) (sublist (string-split mult) (string-split x))))
                                (eval (slice (string-split x) (+ 2 (sublist (string-split mult) (string-split x))) ))))
                           )]
                      ))
              line))
       exp-list))


  
;format dialogue (body) 
(define (format-dialogue lst)
  (if (equal? (length lst) 2) (list lst)
      (cons (take lst 2)
            (format-dialogue (rest (rest lst))))))

;evaluate dialogue
(define (evaluate-dialogue lst descs settings c)
  ;loop through each line in the list
  (map (lambda (line)
         ;loop through each argument in each line (i.e. (id expression))
         (map (lambda (x)
                (cond
                  
                  ;return simply the name without the :
                  [(member #\: (string->list x)) (string-replace x ":" "")]

                  ;check function calls
                  [(sublist (string-split call) (string-split x))

                   ;find and call the function
                   ((get-func (list-ref (string-split x) 3) settings) (second (first (evaluate-dialogue (list (list (first line) (string-join (member (list-ref (string-split x) 5) (string-split x))))) descs settings 1)))) 
                   ]
                  
                  ;if add is present, add
                  [(sublist (string-split add) (string-split x))

                   ;expr 1 (left) 
                   (+ (+ (if (or (member (string-replace (first line) ":" "")
                                     (before-index (string-split x) (sublist (string-split add) (string-split x))))
                                 (self-ref-check (before-index (string-split x) (sublist (string-split add) (string-split x)))))
                             (- (second (member (string-replace (first line) ":" "") (flatten descs))) 1)
                             0)
                         (eval (before-index (string-split x) (sublist (string-split add) (string-split x)))))
                      
                      ;expr 2 (right)
                      (+ (if (or (member (string-replace (first line) ":" "")
                                     (member (list-ref (string-split x) (+ 2 (sublist (string-split add) (string-split x)))) (string-split x)))
                                 (self-ref-check (member (list-ref (string-split x) (+ 2 (sublist (string-split add) (string-split x)))) (string-split x))))
                             (- (second (member (string-replace (first line) ":" "") (flatten descs))) 1)
                             0)
                         (eval (slice (string-split x) (+ 2 (sublist (string-split add) (string-split x))) ))))]
 
                  ;if mult is present, mult
                  [(sublist (string-split mult) (string-split x))

                   ;expr 1 (left) 
                   (* (+ (if (or (member (string-replace (first line) ":" "")
                                     (before-index (string-split x) (sublist (string-split mult) (string-split x))))
                                 (self-ref-check (before-index (string-split x) (sublist (string-split mult) (string-split x)))))
                             (- (second (member (string-replace (first line) ":" "") (flatten descs))) 1)
                             0)
                         (eval (before-index (string-split x) (sublist (string-split mult) (string-split x)))))
                      
                      ;expr 2 (right)
                      (+ (if (or (member (string-replace (first line) ":" "")
                                     (member (list-ref (string-split x) (+ 2 (sublist (string-split mult) (string-split x)))) (string-split x)))
                                 (self-ref-check (member (list-ref (string-split x) (+ 2 (sublist (string-split mult) (string-split x)))) (string-split x))))
                             (- (second (member (string-replace (first line) ":" "") (flatten descs))) 1)
                             0)
                         (eval (slice (string-split x) (+ 2 (sublist (string-split mult) (string-split x))) ))))]

                  
                  ;if no special chars are present, simply evaluate normally
                  [else (eval (string-split x))]
                  )) line))
       lst))
                    
;check for self references in list
(define (self-ref-check lst)
  (if (empty? (filter (lambda (ref)
         (member ref lst))
       self-refs))
      #f
      #t))
         
;get procedure from settings
(define (get-func name settings)
  (first(map (lambda (func)
            (if (equal? (string-replace (first func) "," "") name)
                (second func)
                #f))
          settings)))
  
;evaluate descriptions
(define (eval lst)
  (let([b (countbad lst)])
    (if (equal? b 0)
        (length lst)
        (* -1 (expt 2 b) (length lst)))))

;count number of bad words in a lst
(define (countbad lst)
  (length (filter (lambda (word)
                   (member word bad-words))
                 lst)))

;slice a list
(define (slice lst start [end #f])
  (let ([lst0 (drop lst start)])
    (take lst0 (or end (length lst0)))))
  
;<---------------------------------------------exercise code ------------------------------>

;splitter from e3
(define (splitter word lst)
    (if (equal? (sublist (string-split splitter) (string-split lst)) #f) #f      
        (cons(before-index  (string-split lst) (sublist (string-split splitter) (string-split lst)))
             (list (help (string-split splitter) (string-split lst) 0)))))


;my helper for sublist from ex1, returns all items after a sublist match
(define (help sub lst x)
  (let [(original sub)]
    (cond [(empty? sub) lst]
          [(empty? lst) #f]
          [(equal? (first sub) (first lst))      
           (help (rest sub) (rest lst) 1)]
          [(equal? 1 x) (help original lst 0)] 
          [else (help sub (rest lst) 0)])))



;shrey mahendru's sublist from ex1 cause mine is bugged..
(define (sublist sub lst)
  (if (empty? sub)
      0
      (if (not(equal? (member (first sub) lst) #f)) ;if first sublist element is in lst
          ;(if (equal? lst_element (first sub))
          (if (equal? sub (before-index (member (first sub) lst) (length sub)))
              (- (length lst) (length (member (first sub) lst) ))
              ;(printf (member (first sub) (rest (member (first sub) lst))))
              (if (not(equal? (member (first sub) (rest (member (first sub) lst))) #f))
                  (if (not(equal? (sublist sub (rest (member (first sub) lst))) #f))
                      (+ (+ 1 (sublist sub (rest (member (first sub) lst))))
                         (- (length lst) (length (member (first sub) lst))))
                  ;(sublist sub (rest (member (first sub) lst)))
                      #f)
                  #f)
              )#f)))

;helper for sublist, return all items in lst from start until index n
(define (before-index lst n)
    (if (empty? lst)'()
        (if (equal? n 0) '()                       
            (cons (first lst) (before-index (rest lst) (- n 1))))))

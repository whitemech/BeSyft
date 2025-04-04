
(define (problem triangle-tire-test1)
  (:domain triangle-tire)
  (:objects l11 l21 l22 l31 - location)
  (:init (vehicle-at l11)(road l11 l21)(road l21 l31)(road l11 l22)(road l22 l31)(spare-in l21)(not-flattire))
  (:goal (and)))

(define (problem triangle-tire-test)
  (:domain triangle-tire)
  (:objects l11 l21 l22 l31 l32 l41 - location)
  (:init (vehicle-at l11)(road l11 l21)(road l21 l31)(road l31 l41)(road l11 l22)(road l22 l32)(road l32 l41)(spare-in l21)(not-flattire))
  (:goal (and)))
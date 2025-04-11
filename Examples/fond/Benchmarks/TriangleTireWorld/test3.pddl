
(define (problem triangle-tire-test3)
  (:domain triangle-tire)
  (:objects l11 l21 l22 l31 l32 l41 l51 l52 l61 l62 l71 l72 - location)
  (:init (vehicle-at l11)(road l11 l21)(road l21 l31)(road l31 l41)(road l11 l22)(road l22 l32)(road l32 l41)(road l11 l51)(road l51 l61)(road l61 l41)(road l11 l52)(road l52 l62)(road l62 l41)(road l11 l71)(road l71 l72)(road l72 l41)
  (spare-in l32)(not-flattire))
  (:goal (and)))
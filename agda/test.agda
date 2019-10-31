module test where

open import Agda.Builtin.IO

data Fruit : Set where
  apple : Fruit
  orange : Fruit

data Animal : Set where
  dog : Animal
  cat : Animal

id : (A : Set) -> A -> A
id A x = x

-- this only works if the type is some set, not a function on sets
get_type : {A : Set} -> A -> Set
get_type {A} x = A

_∘_ : {A : Set} -- A is the type of the parameter of g 
      {B : A -> Set} -- B is the type of the parameter of f (depends on A) 
      {C : (x : A) -> B x -> Set} -- C is the return type of
                                  -- f (depends on A and B 
      (f : {x : A}(y : B x) -> C x y) -- f is the first function 
      (g : (x : A) -> B x) -- g is the second function it returns B
      (x : A) -> -- x is the parameter
      C x (g x) -- the return type of ∘ is a C that takes the   

(f ∘ g) x = f (g x)

data ℕ : Set where
  zero : ℕ 
  suc : ℕ → ℕ -- placing suc under the definition of Nat means I don't
                   -- need to define it elsewhere, it is part of the Nat
                   -- concept

_+_ : ℕ → ℕ → ℕ 
zero + n = n
suc m + n = suc ( m + n )
Nat = ℕ

_*_ : ℕ → ℕ → ℕ 
zero * m = zero
(suc x) * y = y + (x * y) 

data Vec (A : Set) : Nat -> Set where
  [] : Vec A zero
  _::_ : {n : Nat} -> A -> Vec A n -> Vec A (suc n)

vecadd : {A : Set}{n : Nat}(plus : A -> A -> A) -> Vec A n -> Vec A n -> Vec A n
vecadd _ [] [] = []
vecadd pls ( s1 :: x ) ( s2 :: y ) = ( pls s1 s2 ) :: ( vecadd pls x y )

data List (A : Set) : Set where
  [̸∅] : List A
  _||_ : A -> List A -> List A

apply : (A : Set) -> -- the parameter type
        (B : A -> Set) -> -- the function type
        ((x : A) -> B x) -> -- the function 
        (a : A) -> -- the parameter
        B a -- the result

apply A B f a = f a


one = (suc zero)
two = (suc one)
three = (suc two)
four = (suc three)
five = (suc four)

-- suc has type Nat -> Nat which should be a subset of Nat -> Set but
-- for some reason it isn't
-- ff = apply Nat suc suc four 
vec1 = one :: ( two :: ( three :: [] ) )

{-# BUILTIN NATURAL ℕ #-}

import Relation.Binary.PropositionalEquality as Eq
open Eq using (_≡_; refl)
-- open Eq.≡-Reasoning using (begin_; _≡⟨⟩_; _∎)
open Eq.≡-Reasoning using (begin_; _≡⟨⟩_; _∎)

_ : 2 + 3 ≡ 5 
_ = begin
  2 +  3 ≡⟨⟩
  5
  ∎

_^_ : ℕ → ℕ → ℕ
m ^ 0 = 1
m ^ ( suc n ) = m * ( m ^ n )

_∸_ : ℕ → ℕ → ℕ
m ∸ zero = m
zero ∸ suc n = zero
suc m ∸ suc n = m ∸ n

infixl 6 _+_
infixl 7 _*_
infixl 8 _^_

{-# BUILTIN NATPLUS _+_ #-}
{-# BUILTIN NATTIMES _*_ #-}
{-# BUILTIN NATMINUS _∸_ #-}

data Bin : Set where
  nil : Bin
  x0_ : Bin → Bin
  x1_ : Bin → Bin

inc : Bin → Bin
inc nil = x1_ nil
inc ( x0 x ) = x1 x
inc ( x1 x ) = x0 inc x

to : ℕ → Bin
from : Bin → ℕ

to 0 = x0 nil
to ( suc x ) = inc ( to x )

from nil = 0
from ( x0 x ) = ( from x ) * 2
from ( x1 x ) = ( from x ) * 2 + 1

P1 : ( from ( to 4 ) ) ≡ 4 

P1 = begin 
  from ( to 4 ) ≡⟨⟩ 4
  ∎

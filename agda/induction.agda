module induction where

import Relation.Binary.PropositionalEquality as Eq
open Eq using (_≡_; refl; cong; sym)
open Eq.≡-Reasoning using (begin_; _≡⟨⟩_; _≡⟨_⟩_; _∎)
open import Data.Nat using (ℕ; zero; suc; _+_; _*_; _∸_)

P : ∀ ( m n p : ℕ ) → ( m + n ) + p ≡ m + ( n + p )

P zero n p = begin
  zero + n + p ≡⟨⟩
  n + p ≡⟨⟩ 
  zero + ( n + p ) 
  ∎ 

P ( suc m ) n p = begin
  ( ( suc m ) + n ) + p ≡⟨⟩
  suc ( m + n ) + p ≡⟨⟩
  suc ( m + n ) + p ≡⟨⟩
  suc ( m + n + p )
    ≡⟨ cong suc ( P m n p ) ⟩
  suc ( m + ( n + p ) )
  ∎

C : ∀ ( a b : ℕ ) → a + b ≡ b + a

L1 : ∀ ( n : ℕ ) → n + zero ≡ n

L1 zero = begin
  zero ≡⟨⟩ zero
  ∎

L1 ( suc n ) = begin
  ( suc n ) + zero ≡⟨⟩
  suc ( n + zero ) ≡⟨ cong suc ( L1 n ) ⟩
  suc ( n )
  ∎

L2 : ∀ ( m n : ℕ ) → m + ( suc n ) ≡ suc ( m + n )

L2 zero n = begin
  zero + ( suc n ) ≡⟨⟩
  suc n
  ∎

L2 (suc m) n = begin
  ( suc m ) + ( suc n ) ≡⟨⟩
  ( suc ( m + suc n ) ) ≡⟨ cong suc ( L2 m n ) ⟩
  ( suc ( suc ( m + n ) ) ) ≡⟨⟩
  suc ( ( suc m ) + n ) 
  ∎

C a zero = begin
  a + zero ≡⟨ L1 a ⟩
  a ≡⟨⟩
  zero + a
  ∎

C a ( suc b ) = begin
  a + ( suc b ) ≡⟨ L2 a b ⟩
  suc ( a + b ) ≡⟨ cong suc ( C a b ) ⟩
  suc ( b + a ) ≡⟨⟩
  ( suc b ) + a
  ∎

R : ∀ ( m n p q : ℕ ) → (m + n) + (p + q) ≡ m + (n + p) + q

R m n p q = begin
  ( m + n ) + ( p + q ) ≡⟨ P m n ( p + q ) ⟩
  m + ( n + ( p + q ) ) ≡⟨ cong ( _+_ m ) (sym ( P n p q ) ) ⟩
  m + ( ( n + p ) + q ) ≡⟨ sym ( P m ( n + p ) q ) ⟩
  m + ( n + p ) + q 
  ∎

P2 : ∀ ( m n p : ℕ ) → ( m + n ) + p ≡ m + ( n + p )
P2 zero n p = refl
P2 (suc m) n p rewrite P2 m n p = refl

+-swap : ∀ ( m n p : ℕ ) → m + (n + p) ≡ n + (m + p)

+-swap m n p = begin
  m + ( n + p ) ≡⟨ sym ( P m n p ) ⟩
  ( m + n ) + p ≡⟨ cong ( _+ p ) ( C m n ) ⟩
  ( n + m ) + p ≡⟨ P n m p ⟩
  n + ( m + p )
  ∎

A : ∀ (m p : ℕ) → (suc m) * p ≡ p + m * p
A m p = refl

*-distrib-+ : ∀ ( m n p : ℕ ) → (  m + n ) * p ≡ ( m * p ) + ( n * p )
*-distrib-+ zero n p = refl
*-distrib-+ (suc m) n p = begin
  ( ( suc m ) + n ) * p ≡⟨⟩
  ( suc ( m + n ) ) * p ≡⟨⟩
  p + ( m + n ) * p ≡⟨ cong (p +_) ( *-distrib-+ m n p ) ⟩
  p + ( m * p + n * p ) ≡⟨ sym ( P p (m * p) (n * p) ) ⟩
  ( p + m * p ) + ( n * p )
    ≡⟨ cong ( _+ n * p ) ( A m p ) ⟩
  ( ( suc m ) * p ) + ( n * p )
  ∎

*-assoc : ∀ ( m n p : ℕ ) → ( m * n ) * p ≡ m * ( n * p )
*-assoc zero n p = refl
*-assoc (suc m) n p = begin
  ( ( suc m ) * n ) * p ≡⟨⟩
  ( n + ( m * n ) ) * p ≡⟨ *-distrib-+ n (m * n) p ⟩
  ( ( n * p ) + ( m * n ) * p )
    ≡⟨ cong (( n * p ) +_) ( *-assoc m n p )  ⟩
  ( ( n * p ) + m * ( n * p ) ) ≡⟨⟩
  ( suc m ) * ( n * p )
  ∎

L3 : ∀ ( m : ℕ ) → m * zero ≡ zero
L3 zero = refl 
L3 (suc m) = begin
  (suc m) * zero ≡⟨⟩
  zero + m * zero ≡⟨ cong ( zero +_ ) ( L3 m ) ⟩
  zero
  ∎

A2 : ∀ ( m n : ℕ ) → ( suc m ) + n ≡ ( suc n ) + m
A2 m n = begin
  ( suc m ) + n ≡⟨⟩
  suc ( m + n ) ≡⟨ cong suc ( C m n ) ⟩
  suc ( n + m ) ≡⟨⟩
  ( suc n ) + m
  ∎

*-comm-part1 : ∀ ( m n : ℕ ) → ( suc n ) * ( suc m )
                             ≡ ( suc m ) + ( n + ( m * n ) )

*-comm-part2 : ∀ ( m n : ℕ ) → ( suc m ) + ( n + ( m * n ) ) ≡
                               ( ( suc n ) + m ) + ( m * n )

*-comm : ∀ ( m n : ℕ ) → m * n ≡ n * m
*-comm zero zero = refl
*-comm (suc m) zero = begin
  (suc m) * zero ≡⟨ L3 (suc m) ⟩
  zero ≡⟨⟩
  zero * (suc m)
  ∎

*-comm zero (suc m) = begin
  zero * (suc m ) ≡⟨⟩
  zero ≡⟨ sym ( L3 ( suc m ) ) ⟩
  (suc m) * zero
  ∎
  

*-comm (suc n) (suc m) = begin
  (suc n) * (suc m) ≡⟨ *-comm-part1 m n ⟩
  (suc m) + ( n + ( m * n ) ) ≡⟨ *-comm-part2 m n ⟩ 
  ( (suc n) + m ) + ( m * n ) ≡⟨ cong ( _+_ ((suc n ) + m )) ( *-comm m n ) ⟩
  ( (suc n) + m ) + ( n * m ) ≡⟨ P2 (suc n) m (n * m) ⟩ 
  ( suc n ) + ( m + ( n * m ) ) ≡⟨⟩ 
  (suc n) + ( ( suc n ) * m ) ≡⟨ cong ( _+_ (suc n) ) ( *-comm (suc n) m ) ⟩
  (suc n) + ( m * ( suc n ) ) ≡⟨⟩
  ( ( suc m ) * ( suc n ) )
  ∎

A3 : ∀ ( m n : ℕ ) → ( ( suc m ) * n ) ≡ n + ( m * n )
A3 m n = refl

*-comm-part1 m n = begin
  (suc n) * (suc m) ≡⟨⟩
  (suc m) + (n * (suc m ) ) ≡⟨ cong ( ( suc m ) +_ ) ( *-comm n ( suc m ) ) ⟩
  (suc m) + ( (suc m) * n ) ≡⟨ cong ( ( suc m ) +_ ) ( A3 m n ) ⟩
  ( suc m ) + ( n + ( m * n ) )
  ∎ 

*-comm-part3 : ∀ ( m n : ℕ ) → ( ( suc m ) + n ) + ( m * n ) 
                             ≡ ( ( suc n ) + m ) + ( m * n ) 

+-part-3 : ℕ → ( ℕ → ℕ )
+-part-3 x = λ { y → y + x }

*-comm-part3 m n = begin
  ( ( suc m ) + n ) + ( m * n ) ≡⟨ cong ( +-part-3 ( m * n ) ) ( A2 m n ) ⟩
  ( ( suc n ) + m ) + ( m * n ) 
  ∎

*-comm-part2 m n = begin
  ( suc m ) + ( n + ( m * n ) ) ≡⟨ sym ( P2 ( suc m ) n ( m * n ) ) ⟩  
  ( ( suc m ) + n ) + ( m * n ) ≡⟨ *-comm-part3 m n ⟩
  ( ( suc n ) + m ) + ( m * n )
  ∎

∸-left-id : ∀ ( n : ℕ ) → zero ∸ n ≡ zero

∸-left-id zero = refl
∸-left-id (suc n) = refl

∸-assoc : ∀ ( m n p : ℕ ) → m ∸ ( n + p ) ≡ m ∸ n ∸ p

∸-assoc zero n p = begin
  zero ∸ ( n + p ) ≡⟨ ∸-left-id ( n + p ) ⟩
  zero ≡⟨ sym ( ∸-left-id p ) ⟩
  zero ∸ p ≡⟨ cong ( _∸ p ) ( sym ( ∸-left-id n ) ) ⟩
  zero ∸ n ∸ p
  ∎

∸-assoc (suc m) (suc n) p = begin
  (suc m) ∸ ( ( suc n ) + p ) ≡⟨⟩
  (suc m) ∸ ( suc (n + p) ) ≡⟨⟩
  m ∸ ( n + p ) ≡⟨ ∸-assoc m n p ⟩
  m ∸ n ∸ p ≡⟨⟩
  (suc m) ∸ (suc n) ∸ p
  ∎

∸-assoc (suc m) zero p = begin
  (suc m) ∸ ( zero + p ) ≡⟨⟩
  (suc m) ∸ p ≡⟨⟩
  (suc m) ∸ zero ∸ p
  ∎

_^_ : ℕ → ℕ → ℕ
m ^ 0 = 1
m ^ ( suc n ) = m * ( m ^ n )

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

^-dist-+ : ∀ ( m n p : ℕ ) → m ^ ( n + p ) ≡ ( m ^ n ) * ( m ^ p )
*-dist-^ : ∀ ( m n p : ℕ ) → ( m * n ) ^  p ≡ ( m ^ p ) * ( n ^ p )
^-dist-* : ∀ ( m n p : ℕ ) → m ^ ( n * p ) ≡ ( m ^ n ) ^ p 

*-identᴿ : ∀ ( n : ℕ ) → n * 1 ≡ n
*-identᴿ zero = refl
*-identᴿ ( suc n ) = begin
  ( suc n ) * 1 ≡⟨⟩
  1 + ( n * 1 ) ≡⟨ cong ( 1 +_ ) ( *-identᴿ n ) ⟩
  1 + n ≡⟨⟩
  suc n
  ∎

^-dist-+ m n zero = begin
  m ^ ( n + zero ) ≡⟨ cong ( m ^_ ) (C n zero) ⟩
  m ^ ( zero + n ) ≡⟨⟩
  m ^ n ≡⟨ sym ( *-identᴿ ( m ^ n ) ) ⟩
  ( m ^ n ) * 1 ≡⟨⟩
  ( m ^ n ) * ( m ^ zero )
  ∎

^-dist-+ m p ( suc n ) = begin
  m ^ ( p + ( suc n ) ) ≡⟨ cong ( m ^_ ) ( C p ( suc n ) ) ⟩
  m ^ ( ( suc n ) + p ) ≡⟨⟩
  m ^ ( suc ( n + p ) ) ≡⟨⟩
  m * m ^ ( n + p ) ≡⟨ cong ( m *_ ) ( ^-dist-+ m n p ) ⟩
  m * ( m ^ n * m ^ p ) ≡⟨ sym ( *-assoc m ( m ^ n ) ( m ^ p ) ) ⟩
  ( m * m ^ n ) * m ^ p ≡⟨⟩
  m ^ ( suc n ) * m ^ p ≡⟨ *-comm ( m ^ suc n ) ( m ^ p ) ⟩
  m ^ p * m ^ ( suc n ) 
  ∎

*-dist-^ m n zero = begin
  ( m * n ) ^ zero ≡⟨⟩
  1 ≡⟨⟩
  1 * 1 ≡⟨⟩
  ( m ^ zero ) * 1 ≡⟨⟩
  ( m ^ zero ) * ( n ^ zero )
  ∎


*-dist-^ m n ( suc p ) = begin
  ( m * n ) ^ ( suc p ) ≡⟨⟩
  ( m * n ) * ( m * n ) ^ p ≡⟨ cong ( ( m * n ) *_ ) ( *-dist-^ m n p ) ⟩
  ( m * n ) * ( ( m ^ p ) * ( n ^ p ) )
    ≡⟨ sym ( *-assoc ( m * n ) (m ^ p ) (n ^ p) ) ⟩
  ( ( m * n ) * m ^ p ) * ( n ^ p )
    ≡⟨ *-comm ( ( m * n ) * m ^ p ) ( n ^ p ) ⟩
  ( n ^ p ) * ( ( m * n ) * ( m ^ p ) )
    ≡⟨ cong ( ( n ^ p ) *_ ) ( *-assoc m n ( m ^ p ) ) ⟩
  ( n ^ p ) * ( m * ( n * ( m ^ p ) ) ) 
    ≡⟨ sym ( *-assoc (n ^ p) m ( n * m ^ p ) ) ⟩
  ( ( n ^ p ) * m ) * ( n * ( m ^ p ) ) 
    ≡⟨ cong ( (( n ^ p ) * m) *_) ( *-comm n ( m ^ p ) ) ⟩
  ( ( n ^ p ) * m ) * ( ( m ^ p ) * n )
    ≡⟨ *-assoc ( n ^ p ) m ( ( m ^ p ) * n ) ⟩
  ( n ^ p ) * (m * ( ( m ^ p ) * n  ) )
    ≡⟨ cong ( n ^ p *_ ) ( sym ( *-assoc m ( m ^ p ) n ) ) ⟩
  ( n ^ p ) * (m * ( m ^ p ) * n )
    ≡⟨ *-comm (n ^ p) ( m * ( m ^ p ) * n ) ⟩
  m * ( m ^ p ) * n * ( n ^ p ) ≡⟨⟩
  ( m * m ^ p ) * n * n ^ p ≡⟨ *-assoc ( m * m ^ p ) n ( n ^ p ) ⟩
  (m * m ^ p ) * ( n * n ^ p ) ≡⟨⟩
  ( m ^ ( suc p ) ) * (n * n ^ p ) ≡⟨⟩
  ( m ^ ( suc p ) ) * ( n ^ ( suc p ) )
  ∎

A4 : ∀ ( n : ℕ ) → n ^ zero ≡ 1
A4 n = begin
  n ^ zero ≡⟨⟩
  1
  ∎

^-dist-* m n zero = begin
  m ^ ( n * zero ) ≡⟨ cong ( m ^_ ) ( *-comm n zero ) ⟩
  m ^ ( zero * n ) ≡⟨⟩
  m ^ zero ≡⟨⟩
  1 ≡⟨ sym ( A4 ( m ^ n ) ) ⟩
  ( m ^ n ) ^ zero
  ∎

^-dist-* m n ( suc p ) = begin
  m ^ ( n * ( suc p ) ) ≡⟨ cong ( m ^_ ) ( *-comm n ( suc p ) ) ⟩
  m ^ ( ( suc p ) * n ) ≡⟨⟩
  m ^ ( n + ( p * n ) ) ≡⟨ ^-dist-+ m n ( p * n ) ⟩
  ( m ^ n ) * ( m ^ ( p * n ) )
    ≡⟨ cong ( λ { z → (m ^ n) * m ^ z } ) ( *-comm p n ) ⟩
  ( m ^ n ) * ( m ^ ( n * p ) )
    ≡⟨ cong ( ( m ^ n ) *_ ) ( ^-dist-* m n p ) ⟩
  ( m ^ n ) * ( ( m ^ n ) ^ p ) ≡⟨⟩
  ( m ^ n ) ^ ( suc p )
  ∎

Bin-inc-suc : ∀ ( x : Bin ) → from ( inc x ) ≡ suc ( from x )

-- Bin-to-from : ∀ ( x : Bin ) → to ( from x ) ≡ x

Bin-from-to : ∀ ( x : ℕ ) → from ( to x ) ≡ x

Bin-inc-suc nil = refl
Bin-inc-suc ( x0 x ) = begin
  from ( inc ( x0 x ) ) ≡⟨⟩
  from ( x1 x ) ≡⟨⟩
  from ( x ) * 2 + 1 ≡⟨⟩
  from ( x0 x ) + 1 ≡⟨ C ( from (x0 x) ) 1 ⟩
  1 + from ( x0 x ) ≡⟨⟩
  suc ( zero + from ( x0 x ) ) ≡⟨⟩
  suc ( from ( x0 x ) )
  ∎

Bin-inc-suc ( x1 x ) = begin
  from ( inc ( x1 x ) ) ≡⟨⟩
  from ( x0 ( inc x ) ) ≡⟨⟩
  from ( inc x ) * 2 ≡⟨ cong ( _* 2 ) ( Bin-inc-suc x ) ⟩ 
  ( suc (from x) ) * 2 ≡⟨⟩
  ( 1 + (from x) ) * 2 ≡⟨ *-distrib-+ 1 (from x) 2 ⟩
  1 * 2 + (from x) * 2 ≡⟨⟩
  2 + (from x) * 2 ≡⟨⟩
  1 + 1 + (from x) * 2 ≡⟨ cong ( 1 +_ ) ( C 1 ( (from x) * 2) ) ⟩ 
  1 + (from x) * 2 + 1 ≡⟨⟩
  1 + (from (x1 x)) ≡⟨⟩
  suc ( from (x1 x) ) 
  ∎
  
-- Bin-to-from is false to (from nil) ≡ x0 nil
-- any number of leading zeros will be replaces by exactly 1

Bin-from-to zero = begin
  from ( to zero ) ≡⟨⟩
  from ( x0 nil ) ≡⟨⟩
  ( from nil ) * 2 ≡⟨⟩
  zero * 2 ≡⟨⟩
  zero
  ∎

Bin-from-to (suc n) = begin
  from ( to ( suc n ) ) ≡⟨⟩
  from ( inc ( to n ) ) ≡⟨ Bin-inc-suc ( to n ) ⟩
  suc ( from ( to n ) ) ≡⟨ cong suc ( Bin-from-to n  ) ⟩
  suc n
  ∎
    

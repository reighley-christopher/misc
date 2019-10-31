module relations where

import Relation.Binary.PropositionalEquality as Eq
open Eq using (_≡_; refl; cong; sym ; naturality )
open Eq.≡-Reasoning using ( begin_ ; _≡⟨⟩_ ; _≡⟨_⟩_ ; _∎ )
open import Data.Nat using (ℕ; zero; suc; _+_; _*_)
open import Data.Nat.Properties using (+-comm ; *-comm ; +-assoc ; *-distrib-+ )
import induction  
open induction using ( Bin ; x0_ ; x1_ ; nil ; inc ; from ; to )

data _≤_ : ℕ → ℕ → Set where

  z≤n : ∀ { n : ℕ }
    → zero ≤ n

  s≤s : ∀ { m n : ℕ }
    → m ≤ n
    → suc m ≤ suc n


_ : 2 ≤ 4
_ = s≤s ( s≤s z≤n )

infix 4 _≤_

inv-s≤s : ∀ { m n : ℕ }
  → suc m ≤ suc n
  → m ≤ n

inv-s≤s (s≤s m≤n) = m≤n

inv-z≤n : ∀ { m : ℕ } → m ≤ zero → m ≡ zero

inv-z≤n z≤n = refl

≤-refl : ∀ { n : ℕ } → n ≤ n

≤-refl {zero} = z≤n
≤-refl {suc n} = s≤s ≤-refl

≤-trans : ∀ { m n p : ℕ } → m ≤ n → n ≤ p → m ≤ p

≤-trans z≤n _ = z≤n
≤-trans (s≤s m≤n) (s≤s n≤p) = s≤s ( ≤-trans m≤n n≤p )

≤-antisym : ∀ { m n : ℕ } → m ≤ n → n ≤ m → m ≡ n

≤-antisym z≤n z≤n = refl

≤-antisym ( s≤s m≤n ) ( s≤s n≤m ) = cong suc ( ≤-antisym m≤n n≤m )

data Total (m n : ℕ) : Set where
  forward : m ≤ n → Total m n
  flipped : n ≤ m → Total m n

≤-total : ∀ ( m n : ℕ ) → Total m n

≤-total zero n = forward z≤n
≤-total (suc m) zero = flipped z≤n
≤-total (suc m) (suc n) with ≤-total m n
...  | forward m≤n = forward (s≤s m≤n )
...  | flipped n≤m = flipped (s≤s n≤m )

+-monoᴿ-≤ : ∀ ( n p q : ℕ ) → p ≤ q → n + p ≤ n + q

+-monoᴿ-≤ zero p q p≤q = p≤q
+-monoᴿ-≤ ( suc n ) p q p≤q = s≤s ( +-monoᴿ-≤ n p q p≤q )

+-monoᴸ-≤ : ∀ ( m n p : ℕ ) → m ≤ n → m + p ≤ n + p 

+-monoᴸ-≤ m n p m≤n rewrite +-comm m p | +-comm n p = +-monoᴿ-≤ p m n m≤n

+-mono-≤ : ∀ ( m n p q : ℕ ) → m ≤ n → p ≤ q → m + p ≤ n + q

+-mono-≤ m n p q m≤n p≤q =
  ≤-trans (+-monoᴸ-≤ m n p m≤n ) (+-monoᴿ-≤ n p q p≤q) 

-- mono-L looks at m n p and the fact that m <= n
-- this becomes evidence that m + p <= n + p
-- mono-R looks at n p q and the fact that p <= q
-- this becomes evidence that n + p <= n + q
-- these two facts fed into trans show that m + p < n + q qed

*-mono-≤ : ∀ ( m n p q : ℕ ) → m ≤ n → p ≤ q → m * p ≤ n * q

*-monoᴿ-≤ : ∀ ( n p q : ℕ ) → p ≤ q → n * p ≤ n * q

*-monoᴿ-≤ zero p q p≤q = z≤n

-- p≤q  : p <= q
-- z≤n  : zero <= n for all n

-- suc(n) * p <= suc(n) * q
-- p + ( n * p ) <= q + ( n * q )

*-monoᴿ-≤ ( suc n ) p q p≤q =
  (
  +-mono-≤
    p
    q
    ( n * p )
    ( n * q )
    p≤q
    ( *-monoᴿ-≤ n p q p≤q )
  )

*-monoᴸ-≤ : ∀ ( m n p : ℕ ) → m ≤ n → m * p ≤ n * p 

*-monoᴸ-≤ m n p m≤n rewrite *-comm m p | *-comm n p = *-monoᴿ-≤ p m n m≤n

*-mono-≤ m n p q m≤n p≤q =
  ≤-trans ( *-monoᴸ-≤ m n p m≤n ) (*-monoᴿ-≤ n p q p≤q) 

infix 4 _<_

data _<_ : ℕ → ℕ → Set where
  z<s : ∀ { n : ℕ } → zero < suc n
  s<s : ∀ { n m : ℕ } → m < n → suc m < suc n

<-trans : ∀ { m n p : ℕ } → m < n → n < p → m < p

s≤→< : ∀ { m n : ℕ } → suc m ≤ n → m < n

<→s≤ : ∀ { m n : ℕ } → m < n → suc m ≤ n

s≤→< (s≤s ( z≤n ) ) = z<s
s≤→< (s≤s (s≤s ( smn ) ) ) = s<s (s≤→< ( s≤s ( smn ) ) )
<→s≤ ( z<s ) = s≤s ( z≤n )
<→s≤ ( s<s m<n ) =  s≤s ( <→s≤ m<n )  

-- <-trans ( s≤→< sm<=n ) ( s≤→< sn<=p )
--  = s≤→< ( ≤-trans ( s≤s sm<=n ) ( sn<=p )) 

n<=sn : ∀ { n : ℕ } → n ≤ suc n
n<=sn { zero } = z≤n
n<=sn { suc n } = s≤s ( n<=sn { n } )

<-trans ( m<n ) n<p
  = s≤→<
      ( ≤-trans
      ( n<=sn )  
      ( ≤-trans
        (
        <→s≤ ( s<s m<n )
        )
        ( <→s≤ n<p
        )
      ))

-- given m < n and n < p
-- suc m < suc n
-- then suc suc m <= suc n and suc n <= p
-- then suc suc m <= p
-- then suc m < p

data Trichotomy (m n : ℕ) : Set where
  trich-left : m < n → Trichotomy m n
  trich-center : m ≡ n → Trichotomy m n
  trich-right : n < m → Trichotomy m n

<-trichotomy : ∀ (m n : ℕ) → Trichotomy m n

<-trichotomy zero ( suc n ) = trich-left z<s
<-trichotomy ( suc n ) zero = trich-right z<s
<-trichotomy zero zero = trich-center refl
<-trichotomy ( suc m ) ( suc n ) with <-trichotomy m n
...  | trich-left m≤n = trich-left (s<s m≤n )
...  | trich-center m≡n = trich-center ( cong suc m≡n) 
...  | trich-right n≤m = trich-right (s<s n≤m )

+-mono-< : ∀ ( m n p q : ℕ ) → m < n → p < q → m + p < n + q

A1 : ∀ ( x y : ℕ ) → ( suc x ) + ( suc y ) ≡ ( suc ( suc ( x + y ) ) )
A1 x y = begin
  ( suc x ) + ( suc y ) ≡⟨⟩
  suc ( x + ( suc y ) ) ≡⟨ cong suc ( +-comm x ( suc y ) ) ⟩
  suc ( ( suc y ) + x ) ≡⟨⟩
  suc ( suc ( y + x ) ) ≡⟨ cong ( λ { x → ( suc ( suc x ) ) } ) ( +-comm  y x ) ⟩
  suc ( suc ( x + y ) ) 
  ∎

-- A2 : ∀ ( x y z : ℕ ) → ( suc x ) + ( suc y ) < z
--                     → suc ( suc ( x + y ) ) < z 

A3 : { A B : Set } → A ≡ B → B → A → B

A3 ab b a = b

-- A2 x y z sxsy = A3 ( A1 x y ) ( suc ( suc ( x + y ) ) < z ) ( ( suc x ) + ( suc y ) < z ) sxsy

-- B1 : ∀ ( x y z : ℕ ) → ( suc x ) + y < z
--                     → suc ( x + y ) < z

B2 : ∀ ( x y : ℕ ) → ( suc x ) + y ≡ suc ( x + y )
B2 x y = begin
  ( suc x ) + y ≡⟨⟩ suc ( x + y )
  ∎

--B1 x y z rewrite B2 x y = s<s  

-- cong takes a function and returns an equivalence
-- what if I have the equivalence and I need the function??

-- +-mono-< m n p q m<n p<q =
--   s≤→<
--   ( ≤-trans
--     (
--       n<=sn 
--     )
--     ( 
--     +-mono-≤ (suc m) n (suc p) q 
--       (<→s≤ m<n)
--       (<→s≤ p<q)
--     ) 
--   )

-- +-mono-<= takes 4 variables and 2 propositions and returns a proposition
-- but its the wrong one.
-- I need a function which takes those 4 variables and the new proposition
-- and returns the correct proposition (A2) but I can't prove it
-- I can prove A1, which is an equivalence.

-- given m < n , p < q
-- suc m <= n, suc p <= q
-- suc m + suc p <= n + q
-- < becaus suc ( m + p ) <= suc m + suc p

+-monoᴿ-< : ∀ ( n p q : ℕ ) → p < q → n + p < n + q

+-monoᴿ-< zero p q p<q = p<q
+-monoᴿ-< ( suc n ) p q p≤q = s<s ( +-monoᴿ-< n p q p≤q )

+-monoᴸ-< : ∀ ( m n p : ℕ ) → m < n → m + p < n + p 

+-monoᴸ-< m n p m<n rewrite +-comm m p | +-comm n p = +-monoᴿ-< p m n m<n

+-mono-< m n p q m<n p<q =
  <-trans (+-monoᴸ-< m n p m<n ) (+-monoᴿ-< n p q p<q) 

data even : ℕ → Set
data odd : ℕ → Set

data even where
  zero : even zero
  suc : ∀ { n : ℕ } → odd n → even ( suc n )

data odd where
  suc : ∀ { n : ℕ } → even n → odd ( suc n )

e+e≡e : ∀ {m n : ℕ} → even m → even n → even (m + n)

o+e≡o : ∀ {m n : ℕ} → odd m → even n → odd (m + n)

e+e≡e zero en = en
e+e≡e ( suc n ) en = suc ( o+e≡o n en )
o+e≡o (suc em) en = suc ( e+e≡e em en ) 



o+o≡e : ∀ {m n : ℕ} → odd m → odd n → even (m + n)
o+o≡e (suc zero) en = suc en
o+o≡e (suc ( suc m ) ) n = suc ( suc ( o+o≡e m n ) )

data Can : Bin → Set
data One : Bin → Set

data One where
  x1-one : One ( x1 nil )
  x1-x0 : ∀ (x : Bin) → One x → One ( x0 x )
  x1-x1 : ∀ (x : Bin) → One x → One ( x1 x )

data Can where
  x0-can : Can (x0 nil)
  x1-can : ∀ ( x : Bin ) → One x → Can x

inc-can : ∀ ( x : Bin ) → Can x → Can (inc x)
inc-one : ∀ ( x : Bin ) → One x → One (inc x)
to-can : ∀ (n : ℕ ) → Can ( to n )
-- ≡-can : ∀ (x : Bin ) → Can x → to ( from x ) ≡ x

inc-one x x1-one = x1-x0 x x1-one
inc-one ( x0 x ) (x1-x0 x cert) = x1-x1 x cert 
inc-one ( x1 x ) (x1-x1 x cert) = x1-x0 ( inc x ) ( inc-one x cert )

inc-can x x0-can = x1-can ( inc x ) x1-one 
inc-can x (x1-can x cert) = x1-can ( inc x ) ( inc-one x cert )

to-can 0 = x0-can
to-can (suc n) = inc-can ( to n ) ( to-can n )

A4 : ∀ ( n : ℕ ) → ( to ( ( suc n ) * 2 ) ) ≡ ( x0 ( to ( suc n )  ) )


A4 zero = refl
A4 ( suc n ) = begin
   ( to ( ( suc ( suc n ) ) * 2 ) ) ≡⟨⟩
   ( to (  ( 1 + ( suc n ) ) * 2 ) ) ≡⟨⟩
   ( to ( 2 + ( suc n ) * 2 ) ) ≡⟨⟩
   ( to ( 2 + ( suc n ) * 2 ) ) ≡⟨⟩
   ( inc ( to ( 1 + ( suc n ) * 2 ) ) ) ≡⟨⟩
   ( inc ( inc ( to ( ( suc n ) * 2 ) ) ) )
     ≡⟨ cong ( λ { n → ( inc ( inc n ) ) }  ) ( A4 n ) ⟩
   ( inc ( inc ( x0 ( to ( ( suc n ) ) ) ) ) ) ≡⟨⟩
   ( inc ( x1 ( to ( suc n ) ) ) ) ≡⟨⟩
   x0 ( to ( suc ( suc  n ) ) )
   ∎

-- ≡-can ( x0 nil ) x0-can = refl
-- ≡-can ( x1 nil ) ( x1-can ( x1 nil ) x1-one ) = refl 
-- 
-- ≡-can ( x0 ( x1 x ) ) ( x1-can ( x0 ( x1 x ) ) ( x1-x0 ( x1 x ) cert ) ) =
--   begin
--   to ( from ( x0 ( x1 x ) ) ) ≡⟨⟩
--   to ( from ( x1 x ) * 2 ) ≡⟨⟩
--   to ( from ( inc ( x0 x ) ) * 2 ) 
--      ≡⟨ cong ( λ { x → to ( x * 2 ) } ) ( induction.Bin-inc-suc ( x0 x ) ) ⟩
--   to ( suc ( from ( x0 x ) ) * 2 ) ≡⟨ A4 ( from (x0 x ) ) ⟩ 
--   x0 ( to ( suc ( from ( x0 x ) ) ) ) 
--      ≡⟨ cong ( λ { x → x0 ( to x ) } ) ( sym ( induction.Bin-inc-suc ( x0 x ) ) ) ⟩
--   x0 ( to ( from ( inc ( x0 x ) ) ) )  ≡⟨⟩
--   x0 ( to ( from (  x1 x ) ) )
--     ≡⟨ cong x0_ ( ≡-can ( x1 x ) ( x1-can ( x1 x ) cert ) ) ⟩
--   x0 (x1 x)
--   ∎

-- ≡-can x cert with ( from x )
--              ... | ( from nil ) = refl
--              ... | ( from x0 xx ) = refl
--              ... | ( from x1 xx ) = refl

-- ≡-can ( x0 ( x0 x ) ) ( x1-can ( x0 ( x0 x ) ) (x1-x0 (x0 nil ) cert )) =
--   begin
--   to ( from ( x1 x ) ) ) ≡⟨⟩
--   to ( from ( inc ( x0 x )  ) ) ≡⟨⟩
--   to ( suc ( from  ( x0 x ) ) ) ≡⟨⟩
--   to ( suc ( from  ( x ) * 2 ) ) ≡⟨⟩
--   to ( 2 + from  ( x ) * 2  ) ≡⟨⟩
--   x0 (x0 x)
--   ∎



     -- ≡⟨ cong to ( induction.Bin-inc-suc ( x0 ( x1 x ) ) ) ⟩
--             begin
--             to ( from ( x1 x ) ) ≡⟨⟩
--             to ( from ( inc ( x0 x ) ) )
--               ≡⟨ cong to ( induction.Bin-inc-suc (x0 x) ) ⟩
--             to ( suc ( from ( x0 x ) ) ) ≡⟨⟩
--             to ( suc ( from x  ) * 2 ) 
--               ≡⟨ cong to ( A4 ( from x ) ) ⟩
--             x0 ( to ( from x ) )
--               ≡⟨ ( cong ( x0_ ) ( ≡-can x ( x1-can x cert ) ) ) ⟩
--             x0 x
--             ∎

--≡-can ( x1 x ) ( x1-can ( x1 x ) ( x1-x1 x cert ) ) = begin
--             to ( from ( x1 x ) ) ≡⟨⟩
--             to ( from ( inc ( x0 x ) ) )
--               ≡⟨ cong ( to ) ( induction.Bin-inc-suc (x0 x) ) ⟩
--             inc ( to ( from ( x0 x ) ) ) ≡⟨⟩
--             inc ( to ( ( from x ) * 2 ) ) ≡⟨ cong inc ( A4 ( from x ) ) ⟩
--             inc ( x0 ( to ( from x ) ) ) 
--               ≡⟨ ( cong ( λ { x → ( inc ( x0 x ) ) } )
--                ( ≡-can x ( x1-can x cert ) ) ) ⟩
--             inc ( x0 x ) ≡⟨⟩
--             ( x1 x )
--             ∎
 --           
 --            to ( suc ( ( from x ) * 2 ) ) ≡⟨⟩
 --            inc ( to ( from ( x0 x ) ) ) ≡⟨⟩
 --            to ( suc ( ( from x ) * 2 ) ) ≡⟨ A4 ( from x )  ⟩
 --            x0 ( to ( from x ) )
 --              ≡⟨ ( cong ( x0_ ) ( ≡-can x ( x1-can x cert ) ) ) ⟩
 --            x0 x
 --            ∎


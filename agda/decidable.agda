module decidable where

import Relation.Binary.PropositionalEquality as Eq
open Eq using (_≡_; refl ; cong)
open Eq.≡-Reasoning
open import Data.Nat using (ℕ; zero; suc)
open import Data.Product using (_×_) renaming (_,_ to ⟨_,_⟩)
open import Data.Sum using (_⊎_; inj₁; inj₂)
open import Relation.Nullary using (¬_)
open import Relation.Nullary.Negation using ()
  renaming (contradiction to ¬¬-intro)
open import Data.Unit using (⊤; tt)
open import Data.Empty using (⊥; ⊥-elim)
open import relations using (_<_; z<s; s<s)
open import isomorphism using (_⇔_ ; _∘_ )


infix 4 _≤_

data _≤_ : ℕ → ℕ → Set where

  z≤n : ∀ {n : ℕ}
    → zero ≤ n

  s≤s : ∀ {m n : ℕ}
    → m ≤ n
    → suc m ≤ suc n

2≤4 : 2 ≤ 4
2≤4 = s≤s (s≤s z≤n)

¬4≤2 : ¬ (4 ≤ 2)
¬4≤2 (s≤s (s≤s ()))

data Bool : Set where
  true  : Bool
  false : Bool

infix 4 _≤ᵇ_

_≤ᵇ_ : ℕ → ℕ → Bool
zero ≤ᵇ n       =  true
suc m ≤ᵇ zero   =  false
suc m ≤ᵇ suc n  =  m ≤ᵇ n

_ : (2 ≤ᵇ 4) ≡ true
_ =
  begin
    2 ≤ᵇ 4
  ≡⟨⟩
    1 ≤ᵇ 3
  ≡⟨⟩
    0 ≤ᵇ 2
  ≡⟨⟩
    true
  ∎

_ : (4 ≤ᵇ 2) ≡ false
_ =
  begin
    4 ≤ᵇ 2
  ≡⟨⟩
    3 ≤ᵇ 1
  ≡⟨⟩
    2 ≤ᵇ 0
  ≡⟨⟩
    false
  ∎

T : Bool → Set
T true = ⊤
T false = ⊥

T→≡ : ∀ (b : Bool) → T b → b ≡ true
T→≡ true tt   =  refl
T→≡ false ()

≡→T : ∀ {b : Bool} → b ≡ true → T b
≡→T refl  =  tt

≤ᵇ→≤ : ∀ (m n : ℕ) → T (m ≤ᵇ n) → m ≤ n
≤ᵇ→≤ zero    n       tt  =  z≤n
≤ᵇ→≤ (suc m) zero    ()
≤ᵇ→≤ (suc m) (suc n) t   =  s≤s (≤ᵇ→≤ m n t)

≤→≤ᵇ : ∀ {m n : ℕ} → m ≤ n → T (m ≤ᵇ n)
≤→≤ᵇ z≤n        =  tt
≤→≤ᵇ (s≤s m≤n)  =  ≤→≤ᵇ m≤n

data Dec (A : Set) : Set where
  yes :   A → Dec A
  no  : ¬ A → Dec A

¬s≤z : ∀ {m : ℕ} → ¬ (suc m ≤ zero)
¬s≤z ()

¬s≤s : ∀ {m n : ℕ} → ¬ (m ≤ n) → ¬ (suc m ≤ suc n)
¬s≤s ¬m≤n (s≤s m≤n) = ¬m≤n m≤n

_≤?_ : ∀ (m n : ℕ) → Dec (m ≤ n)
zero  ≤? n                   =  yes z≤n
suc m ≤? zero                =  no ¬s≤z
suc m ≤? suc n with m ≤? n
...               | yes m≤n  =  yes (s≤s m≤n)
...               | no ¬m≤n  =  no (¬s≤s ¬m≤n)

¬s<s : ∀ { m n : ℕ } → ¬ (m < n) → ¬ ( suc m < suc n )
¬s<s ¬m<n (s<s m<n) = ¬m<n m<n
¬n<z : ∀ { n : ℕ } → ¬ ( n < zero )
¬n<z ()

_<?_ : ∀ ( m n : ℕ ) → Dec ( m < n )
zero <? ( suc n ) = yes z<s
( suc m ) <? ( suc n ) with ( m <? n )
...  | yes m<n = yes ( s<s m<n )
...  | no ¬m<n = no ( ¬s<s ¬m<n )

m <? zero = no ( ¬n<z )

_≡ℕ?_ : ∀ ( m n : ℕ ) → Dec ( m ≡ n )

A1 : ∀ { m n : ℕ } → suc n ≡ suc m → n ≡ m
A1 refl = refl

zero ≡ℕ? zero = yes refl
( suc n ) ≡ℕ? ( suc m ) with n ≡ℕ? m 
...   | yes n≡m = yes ( cong suc n≡m )
...   | no n≢m = no ( n≢m ∘ A1 ) 

zero ≡ℕ? suc n = no λ()
suc n ≡ℕ? zero = no λ()


_≤?′_ : ∀ (m n : ℕ) → Dec (m ≤ n)
m ≤?′ n with m ≤ᵇ n | ≤ᵇ→≤ m n | ≤→≤ᵇ {m} {n}
...      | true     | p        | _   = yes (p tt)
...      | false    | _        | ¬p  = no ¬p

⌊_⌋ : ∀ {A : Set} → Dec A → Bool
⌊ yes x ⌋  =  true
⌊ no ¬x ⌋  =  false

_≤ᵇ′_ : ℕ → ℕ → Bool
m ≤ᵇ′ n  =  ⌊ m ≤? n ⌋

toWitness : ∀ {A : Set} {D : Dec A} → T ⌊ D ⌋ → A
toWitness {A} {yes x} tt  =  x
toWitness {A} {no ¬x} ()

fromWitness : ∀ {A : Set} {D : Dec A} → A → T ⌊ D ⌋
fromWitness {A} {yes x} _  =  tt
fromWitness {A} {no ¬x} x  =  ¬x x

≤ᵇ′→≤ : ∀ {m n : ℕ} → T (m ≤ᵇ′ n) → m ≤ n
≤ᵇ′→≤  =  toWitness

≤→≤ᵇ′ : ∀ {m n : ℕ} → m ≤ n → T (m ≤ᵇ′ n)
≤→≤ᵇ′  =  fromWitness

infixr 6 _∧_

_∧_ : Bool → Bool → Bool
true  ∧ true  = true
false ∧ _     = false
_     ∧ false = false

infixr 6 _×-dec_

_×-dec_ : ∀ { A B : Set } → Dec A → Dec B → Dec ( A × B )
yes x ×-dec yes y = yes ⟨ x , y ⟩
no ¬x ×-dec _     = no λ{ ⟨ x , y ⟩ → ¬x x }
_     ×-dec no ¬y = no λ{ ⟨ x , y ⟩ → ¬y y }

infixr 5 _∨_

_∨_ : Bool → Bool → Bool
true  ∨ _      = true
_     ∨ true   = true
false ∨ false  = false

infixr 5 _⊎-dec_

_⊎-dec_ : ∀ {A B : Set} → Dec A → Dec B → Dec (A ⊎ B)
yes x ⊎-dec _     = yes (inj₁ x)
_     ⊎-dec yes y = yes (inj₂ y)
no ¬x ⊎-dec no ¬y = no λ{ (inj₁ x) → ¬x x ; (inj₂ y) → ¬y y }

not : Bool → Bool
not true  = false
not false = true

¬? : ∀ {A : Set} → Dec A → Dec (¬ A)
¬? (yes x)  =  no (¬¬-intro x)
¬? (no ¬x)  =  yes ¬x

_⊃_ : Bool → Bool → Bool
_ ⊃ true = true
false ⊃ _ = true
true ⊃ false = false

_→-dec_ : ∀ { A B : Set } → Dec A → Dec B → Dec ( A → B )
_ →-dec yes y = yes ( λ _ → y )
no ¬x →-dec _ = yes ( λ x → ⊥-elim ( ¬x x ) )
yes x →-dec no ¬y  =  no (λ f → ¬y (f x))

∧-× : ∀ {A B : Set} (x : Dec A) (y : Dec B) → ⌊ x ⌋ ∧ ⌊ y ⌋ ≡ ⌊ x ×-dec y ⌋
∨-⊎ : ∀ {A B : Set} (x : Dec A) (y : Dec B) → ⌊ x ⌋ ∨ ⌊ y ⌋ ≡ ⌊ x ⊎-dec y ⌋
not-¬ : ∀ {A : Set} (x : Dec A) → not ⌊ x ⌋ ≡ ⌊ ¬? x ⌋

not-¬ ( yes a ) =
   not ⌊ ( yes a ) ⌋ ≡⟨⟩
   ⌊ ¬? ( yes a ) ⌋ 
   ∎
   
not-¬ ( no a ) =
   not ⌊ ( no a ) ⌋ ≡⟨⟩
   ⌊ ¬? ( no a ) ⌋
   ∎

∧-× ( yes x ) ( yes y ) =
   ( ⌊ ( yes x ) ⌋ ∧ ⌊ ( yes y ) ⌋ ) ≡⟨⟩
   ( ⌊ ( yes x ) ×-dec ( yes y ) ⌋ )
   ∎

∧-× ( no x ) y =
   ( ⌊ ( no x ) ⌋ ∧ ⌊ y ⌋ ) ≡⟨⟩
   ( ⌊ ( no x ) ×-dec ( y ) ⌋ )
   ∎

∧-× ( yes y ) ( no x ) =
   ( ⌊ yes y ⌋ ∧ ⌊ ( no x ) ⌋ ) ≡⟨⟩
   ( ⌊ ( yes y ) ×-dec ( no x ) ⌋ )
   ∎

∨-⊎ ( no x ) ( no y ) =
   ( ⌊ ( no x ) ⌋ ∧ ⌊ ( no y ) ⌋ ) ≡⟨⟩
   ( ⌊ ( no x ) ×-dec ( no y ) ⌋ )
   ∎

∨-⊎ ( yes x ) y =
   ( ⌊ ( yes x ) ⌋ ∨ ⌊ y ⌋ ) ≡⟨⟩
   ( ⌊ ( yes x ) ⊎-dec ( y ) ⌋ )
   ∎

∨-⊎ ( no y ) ( yes x ) =
   ( ⌊ no y ⌋ ∨ ⌊ ( yes x ) ⌋ ) ≡⟨⟩
   ( ⌊ ( no y ) ⊎-dec ( yes x ) ⌋ )
   ∎

_iff_ : Bool → Bool → Bool
_⇔-dec_ : ∀ {A B : Set} → Dec A → Dec B → Dec (A ⇔ B)
iff-⇔ : ∀ {A B : Set} (x : Dec A) (y : Dec B) →
  ⌊ x ⌋ iff ⌊ y ⌋ ≡ ⌊ x ⇔-dec y ⌋

true iff true = true
false iff false = true
true iff false = false
false iff true = false

-- a ⇔-dec b with ( a →-dec b ) | ( b →-dec a )
-- ...                          | yes a→b    | yes b→a  =
-- yes record { to = a→b ; from = b→a }
-- ...                          | no a→b→⊥   | yes b→a  =
-- no λ { record { to = a→b ; from = b→a }
-- → ( a→b→⊥ a→b ) }
-- ...                          | yes a→b   | no b→a→⊥  =
-- no λ { record { to = a→b ; from = b→a }
-- → ( b→a→⊥ b→a ) }
-- ...                          | no a→b→⊥  | no b→a→⊥ ()

--                                yes record
--                                  { to = ⊥-elim  ( a→b→⊥ a b )  
--                                  ; from = ⊥-elim ( b→a→⊥ b a )  
--                                  } 

-- so the big question is where do I geet a function that a→b when all
-- I have is a function a→b→⊥ , which is to say that there is not function
-- a → b, and a function b→a→⊥, so there is no function b to a
-- what I need is to know that there is neither a→b , b→a, but we need it
-- but how does this work as a record
-- ⊥-elim takes a false and then produces some arbitrary result
-- to get a false I will need a function a→b , but all I have is a , b

( yes a ) ⇔-dec ( yes b ) =
  yes record { to = λ { _ → b }; from =  λ { _ → a } }

( no ¬a ) ⇔-dec ( no ¬b ) =
  yes record { to = λ a → ⊥-elim ( ¬a a )
             ; from = λ b → ⊥-elim ( ¬b b ) }

( no ¬a ) ⇔-dec ( yes b ) = no λ { record { to = a→b ; from = b→a } →
                                 ( ¬a ( b→a b ) ) 
                                 } 
( yes a ) ⇔-dec ( no ¬b ) = no λ { record { to = a→b ; from = b→a } →
                                 ( ¬b ( a→b a ) ) 
                                 } 

iff-⇔ ( yes a ) ( yes b ) =
  ⌊ yes a  ⌋ iff ⌊ yes b ⌋ ≡⟨⟩
  ⌊ ( yes a ) ⇔-dec ( yes b )  ⌋
  ∎

iff-⇔ ( no a ) ( no b ) =
  ⌊ no a  ⌋ iff ⌊ no b ⌋ ≡⟨⟩
  ⌊ ( no a ) ⇔-dec ( no b )  ⌋
  ∎

iff-⇔ ( yes a ) ( no b ) =
  ⌊ yes a  ⌋ iff ⌊ no b ⌋ ≡⟨⟩
  ⌊ ( yes a ) ⇔-dec ( no b )  ⌋
  ∎

iff-⇔ ( no a ) ( yes b ) =
  ⌊ no a  ⌋ iff ⌊ yes b ⌋ ≡⟨⟩
  ⌊ ( no a ) ⇔-dec ( yes b )  ⌋
  ∎


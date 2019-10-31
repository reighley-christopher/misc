module negation where

open import Relation.Binary.PropositionalEquality using (_≡_; refl)
open import Data.Nat using (ℕ; zero; suc ; _<_)
open import Data.Empty using (⊥; ⊥-elim)
open import Data.Sum using (_⊎_; inj₁; inj₂)
open import Data.Product using (_×_)
open import isomorphism using (_≃_; extensionality)

¬_ : Set → Set
¬ A = A → ⊥

¬-elim : ∀ {A : Set}
  → ¬ A
  → A
  → ⊥
¬-elim ¬x x = ¬x x

infix 3 ¬_

¬¬-intro : ∀ {A : Set}
  → A
  → ¬ ¬ A

¬¬-intro x  =  λ{¬x → ¬x x}

¬¬-intro′ : ∀ {A : Set}
  → A
  → ¬ ¬ A

¬¬-intro′ x ¬x = ¬x x

¬¬¬-elim : ∀ { A : Set }
  → ¬ ¬ ¬ A → ¬ A

¬¬¬-elim ¬¬¬x = λ { x → ¬¬¬x ( ¬¬-intro x ) }

contraposition : ∀ { A B : Set }
  → (A → B)
  → ( ¬ B → ¬ A )

contraposition a→b ¬b a = ( ¬b ( a→b a ) ) 

_≢_ : ∀ { A : Set } → A → A → Set

x ≢ y = ¬ ( x ≡ y )

_ : 1 ≢ 2
_ = λ()

peano : ∀ { n : ℕ } → zero ≢ suc n
peano = λ()

id : ⊥ → ⊥
id x = x

id′ : ⊥ → ⊥
id′ ()

id≡id′ : id ≡ id′
id≡id′ = extensionality (λ())

assimilation : ∀ {A : Set} (¬x ¬x′ : ¬ A) → ¬x ≡ ¬x′
assimilation ¬x ¬x′ = extensionality (λ x → ⊥-elim (¬x x))

<-irreflexive : ∀ (n : ℕ ) → ¬ ( n < n )
s<s : ∀ ( n m : ℕ ) → n < m → ( suc n ) < ( suc m )
s<s = refl

<-irreflexive zero = λ()
<-irreflexive ( suc n ) = s<s ( <-irreflexive n )

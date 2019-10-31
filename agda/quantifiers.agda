module quantifiers where

import Relation.Binary.PropositionalEquality as Eq
open Eq using (_≡_; refl)
open import Data.Nat using (ℕ; zero; suc; _+_; _*_)
open import Relation.Nullary using (¬_)
open import Data.Product using (_×_; proj₁) renaming (_,_ to ⟨_,_⟩)
open import Data.Sum using (_⊎_)
open import isomorphism using (_≃_; extensionality)

∀-elim : ∀ { A : Set } { B : A → Set }
  → ( L : ∀ ( x : A ) → B x )
  → ( M : A )
  → ( B M )

∀-elim L M = L M

data Σ (A : Set) ( B : A → Set ) : Set where
  ⟨_,_⟩ : ( x : A ) → B x → Σ A B

Σ-syntax = Σ
infix 2 Σ-syntax
syntax Σ-syntax A ( λ x → B ) = Σ[ x ∈ A ] B

∃ : ∀ {A : Set} (B : A → Set) → Set
∃ {A} B = Σ A B

∃-syntax = ∃
syntax ∃-syntax (λ x → B) = ∃[ x ] B

∃-elim : ∀ { A : Set } { B : A → Set } { C : Set }
  → ( ∀ x → B x → C )
  → ∃[ x ] B x
  → C

∃-elim f ⟨ x , y ⟩ = f x y

∀∃-currying : ∀ { A : Set } { B : A → Set } { C : Set }
  → ( ∀ x → B x → C ) ≃ ( ∃[ x ] B x → C )

∀∃-currying =
    record
    { to      =  λ{ f → λ{ ⟨ x , y ⟩ → f x y }}
    ; from    =  λ{ g → λ{ x → λ{ y → g ⟨ x , y ⟩ }}}
    ; from∘to =  λ{ f → refl }
    ; to∘from =  λ{ g → extensionality λ{ ⟨ x , y ⟩ → refl }}
    }

∃-distrib-⊎ : ∀ { A : Set } { B C : A → Set } →
              ∃[ x ] ( B x ⊎ C x ) ≃ ( ∃[ x ] B x ) ⊎ ( ∃[ x ] C x )

data even : ℕ → Set
data odd  : ℕ → Set

data even where

  even-zero : even zero
  
  even-suc : ∀ {n : ℕ}
    → odd n
    → even (suc n)

data odd where
  odd-suc : ∀ {n : ℕ}
    → even n
    → odd (suc n)


even-∃ : ∀ {n : ℕ} → even n → ∃[ m ] (    m * 2 ≡ n)
odd-∃  : ∀ {n : ℕ} →  odd n → ∃[ m ] (1 + m * 2 ≡ n)

even-∃ even-zero =  ⟨ zero , refl ⟩
even-∃ (even-suc o) with odd-∃ o
... | ⟨ m , refl ⟩  =  ⟨ suc m , refl ⟩
 
odd-∃  (odd-suc e)  with even-∃ e
... | ⟨ m , refl ⟩  =  ⟨ m , refl ⟩

module connectives where

import Relation.Binary.PropositionalEquality as Eq
open Eq using (_≡_ ; refl)
open Eq.≡-Reasoning
open import Data.Nat using (ℕ)
open import Function using (_∘_)
open import isomorphism using (  _≃_ ; _≲_ ; extensionality ; _⇔_ )
open isomorphism.≃-Reasoning

data _×_ ( A B : Set ) : Set where

  <_,_> : A → B → A × B

proj₁ : ∀ {A B : Set} → A × B → A
proj₁ < x , y > = x

proj₂ : ∀ {A B : Set} → A × B → B
proj₂ < x , y > = y

η-× : ∀ { A B : Set } ( w : A × B ) → < proj₁ w , proj₂ w > ≡ w
η-× < x , y > = refl

infixr 2 _×_

×-comm : ∀ { A B : Set } → A × B ≃ B × A
×-comm =
  record
    { to = λ{ < x , y > → < y , x > }
    ; from = λ{ < y , x > → < x , y > }
    ; from∘to = λ{ < x , y > → refl }
    ; to∘from = λ{ < y , x > → refl }
    }

×-assoc : ∀ { A B C : Set } → ( A × B ) × C ≃ A × (B × C)
×-assoc =
  record
    { to = λ { < < x , y > , z > → < x , < y , z > > } 
    ; from = λ { <  x , < y , z > > → < < x , y > , z > } 
    ; from∘to = λ { < < x , y > , z > → refl } 
    ; to∘from = λ { < x , < y , z > > → refl }
    }

open _⇔_

-- ⇔≃× : ∀ { A B : Set } →  ( A ⇔ B ) ≃ ( A → B ) × ( B → A )

A_to : ∀ { A B : Set } → ( A ⇔ B ) → ( A → B ) × ( B → A )
A_to a⇔b = < ( a⇔b ).to , ( a⇔b ).from >

A_from : ∀ { A B : Set } → ( A → B ) × ( B → A ) → ( A ⇔ B ) 
A_from a×b = record
    { to = proj₁ a×b 
    ; from = proj₂ a×b 
    }

-- ⇔≃× { A } { B } = record
--   { to = A_to 
--   ; from = A_from
--   ; from∘to = λ { a⇔b   → (
--       begin
--         A_from ( A_to ( a⇔b ) ) ≡⟨⟩
--         record
--           { to = proj₁ ( A_to ( a⇔b ) )
--           ; from = proj₂ ( A_to ( a⇔b ) )
--           } ≡⟨⟩
--         a⇔b
--       ∎ ) } 
--   ; to∘from = λ { ab×ba → (
--       begin
--         A_to ( A_from ( ab×ba ) ) ≡⟨⟩
--         <  ( A_from ( ab×ba ) ).to ,  ( A_from ( ab×ba ) ).from > ≡⟨⟩ 
--         <  ( proj₁ ab×ba ) , ( proj₂ ab×ba ) > 
--       ∎ ) }    
--   }
   

data ⊤ : Set where
  tt : ⊤

η-⊤ : ∀ ( W : ⊤ ) → tt ≡ W
η-⊤ tt = refl

⊤-identityᴸ : ∀ { A : Set } → ⊤ × A ≃ A

⊤-identityᴸ = record
  { to = proj₂
  ; from = λ { A → < tt , A > }
  ; from∘to = λ { < tt , A > → refl } 
  ; to∘from = λ { A → refl } 
  }

data _⊎_ ( A B : Set ) : Set where
  inj₁ : A → A ⊎ B
  inj₂ : B → A ⊎ B

case-⊎ : ∀ { A B C : Set } → ( A → C ) → ( B → C ) → A ⊎ B → C

case-⊎ f g ( inj₁ x ) = f x

case-⊎ f g ( inj₂ y ) = g y

η-⊎ : ∀ { A B : Set } ( w : A ⊎ B ) → case-⊎ inj₁ inj₂ w ≡ w
η-⊎ (inj₁ x) = refl
η-⊎ (inj₂ y) = refl

infix 1 _⊎_

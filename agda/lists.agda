module lists where

import Relation.Binary.PropositionalEquality as Eq
open Eq using (_≡_; refl; sym; trans; cong)
open Eq.≡-Reasoning
open import Data.Bool using (Bool; true; false; T; _∧_; _∨_; not)
open import Data.Nat using (ℕ; zero; suc; _+_; _*_; _∸_; _≤_; s≤s; z≤n)
open import Data.Nat.Properties using
  (+-assoc; +-identityˡ; +-identityʳ; *-assoc; *-identityˡ; *-identityʳ; *-distrib-+ )
open import Relation.Nullary using (¬_; Dec; yes; no)
open import Data.Product using (_×_; ∃; ∃-syntax) renaming (_,_ to ⟨_,_⟩)
open import Data.Sum using ( _⊎_ )
open import Function using (_∘_)
open import Level using (Level)
open import isomorphism using (_≃_; _⇔_)

data List (A : Set) : Set where
  []  : List A
  _∷_ : A → List A → List A

infixr 5 _∷_

_ : List ℕ
_ = 0 ∷ 1 ∷ 2 ∷ 3 ∷ []

{-# BUILTIN LIST List #-}

pattern [_] z = z ∷ []
pattern [_,_] y z = y ∷ z ∷ []
pattern [_,_,_] x y z = x ∷ y ∷ z ∷ []
pattern [_,_,_,_] w x y z = w ∷ x ∷ y ∷ z ∷ []
pattern [_,_,_,_,_] v w x y z = v ∷ w ∷ x ∷ y ∷ z ∷ []
pattern [_,_,_,_,_,_] u v w x y z = u ∷ v ∷ w ∷ x ∷ y ∷ z ∷ []

infixr 5 _++_

_++_ : ∀ {A : Set} → List A → List A → List A
[]       ++ ys  =  ys
(x ∷ xs) ++ ys  =  x ∷ (xs ++ ys)

_ : [ 0 , 1 , 2 ] ++ [ 3 , 4 ] ≡ [ 0 , 1 , 2 , 3 , 4 ]
_ =
  begin
    0 ∷ 1 ∷ 2 ∷ [] ++ 3 ∷ 4 ∷ []
  ≡⟨⟩
    0 ∷ (1 ∷ 2 ∷ [] ++ 3 ∷ 4 ∷ [])
  ≡⟨⟩
    0 ∷ 1 ∷ (2 ∷ [] ++ 3 ∷ 4 ∷ [])
  ≡⟨⟩
    0 ∷ 1 ∷ 2 ∷ ([] ++ 3 ∷ 4 ∷ [])
  ≡⟨⟩
    0 ∷ 1 ∷ 2 ∷ 3 ∷ 4 ∷ []
  ∎

++-assoc : ∀ { A : Set } ( xs ys zs : List A )
  → ( xs ++ ys ) ++ zs ≡ xs ++ ( ys ++ zs )

++-assoc [] ys zs =
  begin
    ( [] ++ ys ) ++ zs
  ≡⟨⟩
    ys ++ zs
  ≡⟨⟩
    [] ++ ( ys ++ zs )
  ∎

++-assoc (x ∷ xs ) ys zs =
  begin
    ( ( x ∷ xs ) ++ ys ) ++ zs
  ≡⟨⟩
    ( x ∷ ( xs ++ ys ) ) ++ zs 
  ≡⟨⟩
    x ∷ ( ( xs ++ ys ) ++ zs )
  ≡⟨ cong ( x ∷_ )  ( ++-assoc xs ys zs ) ⟩
    x ∷ ( xs ++ ( ys ++ zs ) )
  ≡⟨⟩
    ( x ∷ xs ) ++ ( ys ++ zs ) 
  ∎
    
    
++-identityᴸ : ∀ {A : Set} (xs : List A) → [] ++ xs ≡ xs
++-identityᴸ xs =
  begin
    [] ++ xs
  ≡⟨⟩
    xs
  ∎

++-identityᴿ : ∀ {A : Set} (xs : List A) → xs ++ [] ≡ xs

++-identityᴿ [] =
  begin
    [] ++ []
  ≡⟨⟩
    []
  ∎

++-identityᴿ (x ∷ xs) =
  begin
    (x ∷ xs) ++ []
  ≡⟨⟩
    x ∷ (xs ++ [])
  ≡⟨ cong (x ∷_) (++-identityᴿ xs) ⟩
    x ∷ xs
  ∎

length : ∀ {A : Set} → List A → ℕ
length []        =  zero
length (x ∷ xs)  =  suc (length xs)

_ : length [ 0 , 1 , 2 ] ≡ 3
_ =
  begin
    length (0 ∷ 1 ∷ 2 ∷ [])
  ≡⟨⟩
    suc (length (1 ∷ 2 ∷ []))
  ≡⟨⟩
    suc (suc (length (2 ∷ [])))
  ≡⟨⟩
    suc (suc (suc (length {ℕ} [])))
  ≡⟨⟩
    suc (suc (suc zero))
  ∎

length-++ : ∀ {A : Set} (xs ys : List A)
  → length (xs ++ ys) ≡ length xs + length ys

length-++ {A} [] ys =
  begin
    length ([] ++ ys)
  ≡⟨⟩
    length ys
  ≡⟨⟩
    length {A} [] + length ys
  ∎

length-++ (x ∷ xs) ys =
  begin
    length ((x ∷ xs) ++ ys)
  ≡⟨⟩
    suc (length (xs ++ ys))
  ≡⟨ cong suc (length-++ xs ys) ⟩
    suc (length xs + length ys)
  ≡⟨⟩
    length (x ∷ xs) + length ys
  ∎

reverse : ∀ {A : Set} → List A → List A
reverse []        =  []
reverse (x ∷ xs)  =  reverse xs ++ [ x ]

_ : reverse [ 0 , 1 , 2 ] ≡ [ 2 , 1 , 0 ]
_ =
  begin
    reverse (0 ∷ 1 ∷ 2 ∷ [])
  ≡⟨⟩
    reverse (1 ∷ 2 ∷ []) ++ [ 0 ]
  ≡⟨⟩
    (reverse (2 ∷ []) ++ [ 1 ]) ++ [ 0 ]
  ≡⟨⟩
    ((reverse [] ++ [ 2 ]) ++ [ 1 ]) ++ [ 0 ]
  ≡⟨⟩
    (([] ++ [ 2 ]) ++ [ 1 ]) ++ [ 0 ]
  ≡⟨⟩
    (([] ++ 2 ∷ []) ++ 1 ∷ []) ++ 0 ∷ []
  ≡⟨⟩
    (2 ∷ [] ++ 1 ∷ []) ++ 0 ∷ []
  ≡⟨⟩
    2 ∷ ([] ++ 1 ∷ []) ++ 0 ∷ []
  ≡⟨⟩
    (2 ∷ 1 ∷ []) ++ 0 ∷ []
  ≡⟨⟩
    2 ∷ (1 ∷ [] ++ 0 ∷ [])
  ≡⟨⟩
    2 ∷ 1 ∷ ([] ++ 0 ∷ [])
  ≡⟨⟩
    2 ∷ 1 ∷ 0 ∷ []
  ≡⟨⟩
    [ 2 , 1 , 0 ]
  ∎

reverse-++-distrib : ∀ { A : Set } ( xs ys : List A ) →
  reverse ( xs ++ ys ) ≡ reverse ys ++ reverse xs

reverse-++-distrib [] ys =
  begin
    reverse ( [] ++ ys )
  ≡⟨⟩
    reverse ys
  ≡⟨ sym ( ++-identityᴿ ( reverse ys ) ) ⟩
    reverse ys ++ []
  ≡⟨⟩
    reverse ys ++ reverse []
  ∎

reverse-++-distrib ( x ∷ xs ) ys =
  begin
    reverse ( ( x ∷ xs ) ++ ys )
  ≡⟨⟩
    reverse ( x ∷ ( xs ++ ys ) )
  ≡⟨⟩
    reverse ( xs ++ ys ) ++ [ x ]
  ≡⟨ cong ( _++ [ x ] ) ( reverse-++-distrib xs ys ) ⟩
    ( ( reverse ys ) ++ ( reverse xs ) ) ++ [ x ]
  ≡⟨ ++-assoc ( reverse ys ) ( reverse xs ) ( [ x ] ) ⟩
    ( reverse ys ) ++ ( ( reverse xs ) ++ [ x ] )
  ≡⟨ cong ( reverse ys ++_ ) refl ⟩
    ( reverse ys ) ++ ( reverse ( x ∷ xs ) )
  ∎

reverse-involutive : ∀ { A : Set } ( xs : List A ) → 
  reverse ( reverse xs ) ≡ xs

reverse-involutive [] = refl

reverse-involutive ( x ∷ xs ) =
  begin
    reverse ( reverse ( x ∷ xs ) )
  ≡⟨⟩
    reverse ( reverse xs ++ [ x ] )
  ≡⟨ reverse-++-distrib ( reverse xs ) [ x ] ⟩ 
    ( reverse [ x ] ) ++ ( reverse ( reverse xs ) )
  ≡⟨ cong ( ( reverse [ x ] ) ++_ ) ( reverse-involutive xs ) ⟩
   ( reverse [ x ] ) ++ xs
  ≡⟨⟩
    x ∷ xs
  ∎

shunt : ∀ {A : Set} → List A → List A → List A
shunt []       ys  =  ys
shunt (x ∷ xs) ys  =  shunt xs (x ∷ ys)

shunt-reverse : ∀ {A : Set} (xs ys : List A)
  → shunt xs ys ≡ reverse xs ++ ys

shunt-reverse [] ys =
  begin
    shunt [] ys
  ≡⟨⟩
    ys
  ≡⟨⟩
    reverse [] ++ ys
  ∎

shunt-reverse (x ∷ xs) ys =
  begin
    shunt (x ∷ xs) ys
  ≡⟨⟩
    shunt xs (x ∷ ys)
  ≡⟨ shunt-reverse xs (x ∷ ys) ⟩
    reverse xs ++ (x ∷ ys)
  ≡⟨⟩
    reverse xs ++ ([ x ] ++ ys)
  ≡⟨ sym (++-assoc (reverse xs) [ x ] ys) ⟩
    (reverse xs ++ [ x ]) ++ ys
  ≡⟨⟩
    reverse (x ∷ xs) ++ ys
  ∎
                                                            
reverse′ : ∀ {A : Set} → List A → List A
reverse′ xs = shunt xs []

reverses : ∀ {A : Set} (xs : List A)
  → reverse′ xs ≡ reverse xs

reverses xs =
  begin
    reverse′ xs
  ≡⟨⟩
    shunt xs []
  ≡⟨ shunt-reverse xs [] ⟩
    reverse xs ++ []
  ≡⟨ ++-identityᴿ (reverse xs) ⟩
    reverse xs
  ∎

_ : reverse′ [ 0 , 1 , 2 ] ≡ [ 2 , 1 , 0 ]
_ =
  begin
    reverse′ (0 ∷ 1 ∷ 2 ∷ [])
  ≡⟨⟩
    shunt (0 ∷ 1 ∷ 2 ∷ []) []
  ≡⟨⟩
    shunt (1 ∷ 2 ∷ []) (0 ∷ [])
  ≡⟨⟩
    shunt (2 ∷ []) (1 ∷ 0 ∷ [])
  ≡⟨⟩
    shunt [] (2 ∷ 1 ∷ 0 ∷ [])
  ≡⟨⟩
    2 ∷ 1 ∷ 0 ∷ []
  ∎
                                      
map : ∀ {A B : Set} → (A → B) → List A → List B
map f []        =  []
map f (x ∷ xs)  =  f x ∷ map f xs

_ : map suc [ 0 , 1 , 2 ] ≡ [ 1 , 2 , 3 ]
_ =
  begin
    map suc (0 ∷ 1 ∷ 2 ∷ [])
  ≡⟨⟩
    suc 0 ∷ map suc (1 ∷ 2 ∷ [])
  ≡⟨⟩
    suc 0 ∷ suc 1 ∷ map suc (2 ∷ [])
  ≡⟨⟩
    suc 0 ∷ suc 1 ∷ suc 2 ∷ map suc []
  ≡⟨⟩
    suc 0 ∷ suc 1 ∷ suc 2 ∷ []
  ≡⟨⟩
    1 ∷ 2 ∷ 3 ∷ []
  ∎

sucs : List ℕ → List ℕ
sucs = map suc

_ : sucs [ 0 , 1 , 2 ] ≡ [ 1 , 2 , 3 ]
_ =
  begin
    sucs [ 0 , 1 , 2 ]
  ≡⟨⟩
    map suc [ 0 , 1 , 2 ]
  ≡⟨⟩
    [ 1 , 2 , 3 ]
  ∎

map-compose : ∀ ( A B C : Set ) → ( f : A → B ) → ( g : B → C )
  → map (g ∘ f) ≡ map g ∘ map f

map-compose-p : ∀ { A B C : Set } → ( f : A → B ) → ( g : B → C )
  → ( x : List A )
  → ( map (g ∘ f ) ) x ≡ ( map g ∘ map f ) x

map-compose-p f g [] =
  begin
    ( map ( g ∘ f ) ) [] 
  ≡⟨⟩
    [] 
  ≡⟨⟩
    ( map g ∘ map f ) [] 
  ∎

map-compose-p f g ( x ∷ l ) =
  begin
    ( map ( g ∘ f ) ) ( x ∷ l ) 
  ≡⟨⟩
    ( g ∘ f ) x ∷ ( map ( g ∘ f ) ) l
  ≡⟨⟩
    ( g ( f x ) ) ∷ ( map ( g ∘ f ) ) l
  ≡⟨ cong ( g ( f x ) ∷_ ) ( map-compose-p f g l ) ⟩
    ( g ( f x ) ) ∷ ( ( map g ) ∘ ( map f ) ) l
  ≡⟨⟩
    ( g ( f x ) ) ∷ ( map g ( map f l ) )
  ≡⟨⟩
    map g ( ( f x ) ∷ ( map f l ) )
  ≡⟨⟩
    map g ( map f ( x ∷ l ) )
  ≡⟨⟩
    ( map g ∘ map f ) ( x ∷ l )
  ∎

map-++-distribute : { A B : Set } → ( f : A → B ) → ( xs ys : List A ) →
  map f ( xs ++ ys ) ≡ map f xs ++ map f ys

map-++-distribute f [] ys = refl

-- map-++-distribute f ( x ∷ xs ) ys =
-- begin
-- map f ( x ∷ xs ++ ys )
-- ≡⟨⟩
-- map f ( x ∷ ( xs ++ ys ) )
-- ≡⟨⟩
-- f x ∷ map f ( xs ++ ys )
-- ≡⟨ cong ( ( f x ) ∷_ ) ( map-++-distribute f xs ys ) ⟩
-- f x ∷ ( map f xs ++ map f ys )
-- ≡⟨⟩
-- ( f x ∷ map f xs ) ++ map f ys
-- ≡⟨⟩
-- map f ( x ∷ xs ) ++ map f ys
-- ∎

map-++-distribute f ( x ∷ xs ) ys =
  begin
    map f ( x ∷ xs ++ ys )
  ≡⟨ cong ( ( f x ) ∷_ ) ( map-++-distribute f xs ys ) ⟩
    map f ( x ∷ xs ) ++ map f ys
 ∎

data Tree ( A B : Set ) : Set where
  leaf : A → Tree A B
  node : Tree A B → B → Tree A B → Tree A B

map→Tree : ∀ { A B C D : Set } → ( A → C ) → ( B → D ) → Tree A B → Tree C D

map→Tree f g ( leaf a ) = leaf ( f a )

map→Tree f g ( node left b right ) =
  node ( map→Tree f g left ) ( g b ) ( map→Tree f g right )

foldr : ∀ {A B : Set} → (A → B → B) → B → List A → B
foldr _⊗_ e []        =  e
foldr _⊗_ e (x ∷ xs)  =  x ⊗ foldr _⊗_ e xs

_ : foldr _+_ 0 [ 1 , 2 , 3 , 4 ] ≡ 10
_ =
  begin
    foldr _+_ 0 (1 ∷ 2 ∷ 3 ∷ 4 ∷ [])
  ≡⟨⟩
    1 + foldr _+_ 0 (2 ∷ 3 ∷ 4 ∷ [])
  ≡⟨⟩
    1 + (2 + foldr _+_ 0 (3 ∷ 4 ∷ []))
  ≡⟨⟩
    1 + (2 + (3 + foldr _+_ 0 (4 ∷ [])))
  ≡⟨⟩
    1 + (2 + (3 + (4 + foldr _+_ 0 [])))
  ≡⟨⟩
    1 + (2 + (3 + (4 + 0)))
  ∎

sum : List ℕ → ℕ
sum = foldr _+_ 0

_ : sum [ 1 , 2 , 3 , 4 ] ≡ 10
_ =
  begin
    sum [ 1 , 2 , 3 , 4 ]
  ≡⟨⟩
    foldr _+_ 0 [ 1 , 2 , 3 , 4 ]
  ≡⟨⟩
    10
  ∎

product = foldr _*_ 1

foldr-++ : ∀ { A B : Set } → ( _⊗_ : A → B → B )
  → ( e : B ) → ( xs ys : List A )
  → foldr _⊗_ e (xs ++ ys) ≡ foldr _⊗_ (foldr _⊗_ e ys) xs 

foldr-++ _⊗_ e [] ys =
  begin
    foldr _⊗_ e ( [] ++ ys )
  ≡⟨⟩
    foldr _⊗_ e ys
  ≡⟨⟩
    foldr _⊗_ ( foldr _⊗_ e ys ) []
  ∎

foldr-++ _⊗_ e ( x ∷ xs ) ys =
  begin
    foldr _⊗_ e ( ( x ∷ xs ) ++ ys )
  ≡⟨⟩
    foldr _⊗_ e ( x ∷ ( xs ++ ys ) )
  ≡⟨⟩
    x ⊗ foldr _⊗_ e ( xs ++ ys )
  ≡⟨ cong ( x ⊗_ ) ( foldr-++ _⊗_ e xs ys ) ⟩
    x ⊗ foldr _⊗_ ( foldr _⊗_ e ys ) xs
  ≡⟨⟩
    foldr _⊗_ ( foldr _⊗_ e ys ) ( x ∷ xs )
  ∎

foldr-∷ : ∀ { A : Set } → ( xs : List A ) → foldr _∷_ [] xs ≡ xs

foldr-∷ [] = refl

foldr-∷ ( x ∷ xs ) =
  begin
    foldr _∷_ [] ( x ∷ xs )
  ≡⟨⟩
    x ∷ ( foldr _∷_ [] xs )
  ≡⟨ cong ( x ∷_ ) ( foldr-∷ xs ) ⟩
    x ∷ xs
  ∎

foldr-∷-++ : ∀ { A : Set } → ( xs ys : List A ) → xs ++ ys ≡ foldr _∷_ ys xs

foldr-∷-++ [] ys = refl

foldr-∷-++ ( x ∷ xs ) ys =
  begin
    ( x ∷ xs ) ++ ys
  ≡⟨⟩
    x ∷ ( xs ++ ys )
  ≡⟨ cong ( x ∷_ ) ( foldr-∷-++ xs ys ) ⟩
    x ∷ ( foldr _∷_ ys xs )
  ≡⟨⟩
    foldr _∷_ ys ( x ∷ xs ) 
  ∎

extensionality : ∀ {A B : Set} {f g : A → B}
  → (∀ (x : A) → f x ≡ g x)
  → f ≡ g

map-is-foldr-ex : ∀ { A B : Set } → ( f : A → B ) → ( p : List A ) 
  → map f p ≡ foldr ( λ x xs → f x ∷ xs ) [] p

map-is-foldr-ex f [] = refl

map-is-foldr-ex f ( p ∷ ps ) =
  begin
    map f ( p ∷ ps )
  ≡⟨⟩
    ( f p ) ∷ ( map f ps )
  ≡⟨ cong ( ( f p ) ∷_ ) ( map-is-foldr-ex f ps ) ⟩
    ( f p ) ∷ ( foldr ( λ x xs → f x ∷ xs ) [] ps )
  ≡⟨⟩
    ( λ x xs → f x ∷ xs ) p ( foldr ( λ x xs → f x ∷ xs ) [] ps )
  ≡⟨⟩
    foldr ( λ x xs → f x ∷ xs ) [] ( p ∷ ps )
  ∎  
    
fold-Tree : ∀ { A B C : Set } → (A → C) → ( C → B → C → C ) → Tree A B → C

fold-Tree f g ( leaf l ) = f l

fold-Tree f g ( node left n right ) =
  g ( fold-Tree f g left ) n ( fold-Tree f g right)  

map-is-foldr-Tree-ex : ∀ { A B C D : Set } →
  ( f : A → C ) → ( g : B → D ) → ( t : Tree A B ) →
  map→Tree f g t ≡
  fold-Tree ( λ a → leaf ( f a ) ) ( λ cl b cr → node cl ( g b ) cr ) t

map-is-foldr-Tree-ex f g ( leaf a ) = refl

-- map-is-foldr-Tree-ex f g ( node lt n rt ) =
--   begin
--     map→Tree f g ( not lt n rt )
--   ≡⟨⟩
--     node ( map→Tree f g lt ) ( g b ) ( map→Tree f g rt )
--   ≡⟨⟩
--     node (
--       fold-Tree ( λ a → leaf ( f a ) ) ( λ cl b cr → node cl ( g b ) cr ) lt
--     ) ( g b ) (
--       fold-Tree ( λ a → leaf ( f a ) ) ( λ cl b cr → node cl ( g b ) cr ) rt
--     )
--   ≡⟨⟩
--     fold-Tree ( λ a → leaf ( f a ) ) ( λ cl b cr → node cl ( g b ) cr )
--       ( node ( lt ( g b ) rt )

downFrom : ℕ → List ℕ
downFrom zero = []
downFrom ( suc n ) = n ∷ downFrom n

series-sum : ∀ ( n : ℕ ) → sum ( downFrom n ) * 2 ≡ n * ( n ∸ 1 )

series-sum zero = refl

series-sum ( suc n ) =
  sum ( downFrom ( suc n ) ) * 2 ≡⟨⟩
  sum ( n ∷ downFrom n ) * 2 ≡⟨⟩
  ( n + sum ( downFrom n ) ) * 2 ≡⟨ ? ⟩
  n * 2 + ( sum ( downFrom n ) ) * 2 ≡⟨ cong ( n * 2 +_ ) ( series-sum n ) ⟩
  n * 2 + ( n * ( n ∸ 1 ) ) ≡⟨ ? ⟩
  n * ( ( n ∸ 1 ) + 2 ) ≡⟨ ? ⟩
  ( ( suc n ) ∸ 1 ) * ( suc n ) ≡⟨ ? ⟩
  ( suc n ) * ( ( suc n ) ∸ 1 ) 
  ∎
  

record IsMonoid {A : Set} (_⊗_ : A → A → A) (e : A) : Set where
  field
    assoc : ∀ (x y z : A) → (x ⊗ y) ⊗ z ≡ x ⊗ (y ⊗ z)
    identityˡ : ∀ (x : A) → e ⊗ x ≡ x
    identityʳ : ∀ (x : A) → x ⊗ e ≡ x

open IsMonoid

+-monoid : IsMonoid _+_ 0
+-monoid =
  record
    { assoc = +-assoc
    ; identityˡ = +-identityˡ
    ; identityʳ = +-identityʳ
    }

*-monoid : IsMonoid _*_ 1
*-monoid =
  record
    { assoc = *-assoc
    ; identityˡ = *-identityˡ
    ; identityʳ = *-identityʳ
    }

++-monoid : ∀ {A : Set} → IsMonoid {List A} _++_ []
++-monoid =
  record
    { assoc = ++-assoc
    ; identityˡ = ++-identityᴸ
    ; identityʳ = ++-identityᴿ
    }

foldr-monoid : ∀ {A : Set} (_⊗_ : A → A → A) (e : A) → IsMonoid _⊗_ e →
  ∀ (xs : List A) (y : A) → foldr _⊗_ y xs ≡ foldr _⊗_ e xs ⊗ y

foldr-monoid _⊗_ e ⊗-monoid [] y =
  begin
    foldr _⊗_ y []
  ≡⟨⟩
    y
  ≡⟨ sym (identityˡ ⊗-monoid y) ⟩
    (e ⊗ y)
  ≡⟨⟩
    foldr _⊗_ e [] ⊗ y
  ∎

foldr-monoid _⊗_ e ⊗-monoid (x ∷ xs) y =
  begin
    foldr _⊗_ y (x ∷ xs)
  ≡⟨⟩
    x ⊗ (foldr _⊗_ y xs)
  ≡⟨ cong (x ⊗_) (foldr-monoid _⊗_ e ⊗-monoid xs y) ⟩
    x ⊗ (foldr _⊗_ e xs ⊗ y)
  ≡⟨ sym (assoc ⊗-monoid x (foldr _⊗_ e xs) y) ⟩
    (x ⊗ foldr _⊗_ e xs) ⊗ y
  ≡⟨⟩
    foldr _⊗_ e (x ∷ xs) ⊗ y
  ∎

foldr-monoid-++ : ∀ {A : Set} (_⊗_ : A → A → A) (e : A) → IsMonoid _⊗_ e →
  ∀ (xs ys : List A) → foldr _⊗_ e (xs ++ ys) ≡ foldr _⊗_ e xs ⊗ foldr _⊗_ e ys
foldr-monoid-++ _⊗_ e monoid-⊗ xs ys =
  begin
    foldr _⊗_ e (xs ++ ys)
  ≡⟨ foldr-++ _⊗_ e xs ys ⟩
    foldr _⊗_ (foldr _⊗_ e ys) xs
  ≡⟨ foldr-monoid _⊗_ e monoid-⊗ xs (foldr _⊗_ e ys) ⟩
    foldr _⊗_ e xs ⊗ foldr _⊗_ e ys
  ∎

data All {A : Set} (P : A → Set) : List A → Set where
  []  : All P []
  _∷_ : ∀ {x : A} {xs : List A} → P x → All P xs → All P (x ∷ xs)
    
_ : All (_≤ 2) [ 0 , 1 , 2 ]
_ = z≤n ∷ s≤s z≤n ∷ s≤s (s≤s z≤n) ∷ []

data Any {A : Set} (P : A → Set) : List A → Set where
  here  : ∀ {x : A} {xs : List A} → P x → Any P (x ∷ xs)
  there : ∀ {x : A} {xs : List A} → Any P xs → Any P (x ∷ xs)

infix 4 _∈_ _∉_

_∈_ : ∀ {A : Set} (x : A) (xs : List A) → Set
x ∈ xs = Any (x ≡_) xs

_∉_ : ∀ {A : Set} (x : A) (xs : List A) → Set
x ∉ xs = ¬ (x ∈ xs)

_ : 0 ∈ [ 0 , 1 , 0 , 2 ]
_ = here refl

_ : 0 ∈ [ 0 , 1 , 0 , 2 ]
_ = there (there (here refl))

not-in : 3 ∉ [ 0 , 1 , 0 , 2 ]
not-in (here ())
not-in (there (here ()))
not-in (there (there (here ())))
not-in (there (there (there (here ()))))
not-in (there (there (there (there ()))))

All-++-⇔ : ∀ {A : Set} {P : A → Set} (xs ys : List A) →
  All P (xs ++ ys) ⇔ (All P xs × All P ys)

All-++-⇔ xs ys =
  record
    { to       =  to xs ys
    ; from     =  from xs ys
    }
  where

  to : ∀ {A : Set} {P : A → Set} (xs ys : List A) →
    All P (xs ++ ys) → (All P xs × All P ys)
  to [] ys Pys = ⟨ [] , Pys ⟩
  to (x ∷ xs) ys (Px ∷ Pxs++ys) with to xs ys Pxs++ys
  ... | ⟨ Pxs , Pys ⟩ = ⟨ Px ∷ Pxs , Pys ⟩

  from : ∀ { A : Set} {P : A → Set} (xs ys : List A) →
    All P xs × All P ys → All P (xs ++ ys)
  from [] ys ⟨ [] , Pys ⟩ = Pys
  from (x ∷ xs) ys ⟨ Px ∷ Pxs , Pys ⟩ =  Px ∷ from xs ys ⟨ Pxs , Pys ⟩
    
Any-++-⇔ : ∀ {A : Set} {P : A → Set} (xs ys : List A) →
  Any P (xs ++ ys) ⇔ (Any P xs ⊎ Any P ys)

-- Any-++-⇔ xs ys =
--   record
--     { to : to xs ys
--     ; from : from xs ys
--     }
-- 
--   where
--   to : ∀ {A : Set} {P : A → Set} (xs ys : List A) →
--     Any P (xs ++ ys) → (Any P xs ⊎ Any P ys)
--   to [] ys Pys = record {
--   to (x ∷ xs) ys (Px ∷ Pxs++ys) with to xs ys Pxs++ys
--   ... | ⟨ Pxs , Pys ⟩ = ⟨ Px ∷ Pxs , Pys ⟩
-- 
--   from : ∀ { A : Set} {P : A → Set} (xs ys : List A) →
--     All P xs × All P ys → All P (xs ++ ys)
--   from [] ys ⟨ [] , Pys ⟩ = Pys
--   from (x ∷ xs) ys ⟨ Px ∷ Pxs , Pys ⟩ =  Px ∷ from xs ys ⟨ Pxs , Pys ⟩
 

//
//  MealDTO.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

struct MealResponseDTO: Decodable {
    let meals: [MealEntryDTO]
}

struct MealEntryDTO: Decodable {
    let idMeal: String
    let strMeal: String
    let strDrinkAlternate: String?
    let strCategory: String
    let strArea: String
    let strInstructions: String
    let strMealThumb: String
    let strTags: String?
    let strYoutube: String?
    let strSource: String?
    let strImageSource: String?
    let strCreativeCommonsConfirmed: String?
    let dateModified: String?
    // --------------------------
    let strIngredient1: String?
    let strIngredient2: String?
    let strIngredient3: String?
    let strIngredient4: String?
    let strIngredient5: String?
    let strIngredient6: String?
    let strIngredient7: String?
    let strIngredient8: String?
    let strIngredient9: String?
    let strIngredient10: String?
    let strIngredient11: String?
    let strIngredient12: String?
    let strIngredient13: String?
    let strIngredient14: String?
    let strIngredient15: String?
    let strIngredient16: String?
    let strIngredient17: String?
    let strIngredient18: String?
    let strIngredient19: String?
    let strIngredient20: String?
    // --------------------------
    let strMeasure1: String?
    let strMeasure2: String?
    let strMeasure3: String?
    let strMeasure4: String?
    let strMeasure5: String?
    let strMeasure6: String?
    let strMeasure7: String?
    let strMeasure8: String?
    let strMeasure9: String?
    let strMeasure10: String?
    let strMeasure11: String?
    let strMeasure12: String?
    let strMeasure13: String?
    let strMeasure14: String?
    let strMeasure15: String?
    let strMeasure16: String?
    let strMeasure17: String?
    let strMeasure18: String?
    let strMeasure19: String?
    let strMeasure20: String?
    
    subscript(key: String) -> String? {
           switch key {
           case "strIngredient1": return strIngredient1
           case "strIngredient2": return strIngredient2
           case "strIngredient3": return strIngredient3
           case "strIngredient4": return strIngredient4
           case "strIngredient5": return strIngredient5
           case "strIngredient6": return strIngredient6
           case "strIngredient7": return strIngredient7
           case "strIngredient8": return strIngredient8
           case "strIngredient9": return strIngredient9
           case "strIngredient10": return strIngredient10
           case "strIngredient11": return strIngredient11
           case "strIngredient12": return strIngredient12
           case "strIngredient13": return strIngredient13
           case "strIngredient14": return strIngredient14
           case "strIngredient15": return strIngredient15
           case "strIngredient16": return strIngredient16
           case "strIngredient17": return strIngredient17
           case "strIngredient18": return strIngredient18
           case "strIngredient19": return strIngredient19
           case "strIngredient20": return strIngredient20
               
           case "strMeasure1": return strMeasure1
           case "strMeasure2": return strMeasure2
           case "strMeasure3": return strMeasure3
           case "strMeasure4": return strMeasure4
           case "strMeasure5": return strMeasure5
           case "strMeasure6": return strMeasure6
           case "strMeasure7": return strMeasure7
           case "strMeasure8": return strMeasure8
           case "strMeasure9": return strMeasure9
           case "strMeasure10": return strMeasure10
           case "strMeasure11": return strMeasure11
           case "strMeasure12": return strMeasure12
           case "strMeasure13": return strMeasure13
           case "strMeasure14": return strMeasure14
           case "strMeasure15": return strMeasure15
           case "strMeasure16": return strMeasure16
           case "strMeasure17": return strMeasure17
           case "strMeasure18": return strMeasure18
           case "strMeasure19": return strMeasure19
           case "strMeasure20": return strMeasure20
           
           default: return ""
           }
       }
}

//
//  CategoryRepository.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

class CategoryRepository {
    func getCategories() async throws -> [Category] {
        let response = try await MealDataSource.getCategories()
        return CategoryMapper.mapResponseToModels(input: response.categories)
    }
}

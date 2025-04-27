/* Amplify Params - DO NOT EDIT
	ENV
	FUNCTION_SCRAPRESTAURANTCOMMENTS_NAME
	REGION
	STORAGE_RESTAURANTS_ARN
	STORAGE_RESTAURANTS_NAME
	STORAGE_RESTAURANTS_STREAMARN
Amplify Params - DO NOT EDIT */

import crypto from "crypto";
import {
  SecretsManagerClient,
  GetSecretValueCommand,
} from "@aws-sdk/client-secrets-manager";
import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import { PutCommand, DynamoDBDocumentClient } from "@aws-sdk/lib-dynamodb";
import { LambdaClient, InvokeCommand } from "@aws-sdk/client-lambda";

export const handler = async (event) => {
  let locations = [
    "Paris",
    "Lyon",
    "Lille",
    "Bordeaux",
    "Nice",
    "Perpignan",
    "Blois",
    "Tours",
    "Canne",
    "Le Mans",
  ];

  const promises = locations.map(
    async (location) => await getRestaurants(location),
  );

  let rawRestaurants = await Promise.all(promises);
  rawRestaurants = rawRestaurants.flat();

  const lambdaClient = new LambdaClient({ region: "eu-west-3" });
  for (const rawRestaurant of rawRestaurants) {
    if (!rawRestaurant) continue;
    const restaurant = await saveRestaurant(rawRestaurant);
    const command = new InvokeCommand({
      FunctionName: process.env.FUNCTION_SCRAPRESTAURANTCOMMENTS_NAME,
      Payload: JSON.stringify({
        id: restaurant.id,
        url: restaurant.url,
      }),
    });
    lambdaClient.send(command);
  }
};

const getRestaurants = async (location) => {
  const params = new URLSearchParams({
    term: "restaurants",
    location: location,
    limit: 10,
  }).toString();
  const url = `https://api.yelp.com/v3/businesses/search?${params}`;
  const response = await fetch(url, {
    headers: { Authorization: `BEARER ${await getYelpApiKey()}` },
  });
  const data = await response.json();

  return data.businesses;
};

const saveRestaurant = async (rawRestaurant) => {
  const client = new DynamoDBClient({});
  const dynamodb = DynamoDBDocumentClient.from(client);

  const item = {
    id: crypto.randomUUID(),
    yelpId: rawRestaurant.id,
    name: rawRestaurant.name,
    address: rawRestaurant.location?.display_address.join(", "),
    rating: rawRestaurant.rating,
    url: rawRestaurant.url.split("?")[0],
  };

  const command = new PutCommand({
    TableName: process.env.STORAGE_RESTAURANTS_NAME,
    ConditionExpression: "attribute_not_exists(yelpId)",
    Item: item,
  });

  const response = await dynamodb.send(command);
  if (response.$metadata.httpStatusCode == 200) {
    return item;
  }
};

const getYelpApiKey = async () => {
  const client = new SecretsManagerClient({
    region: "eu-west-3",
  });

  let response;

  try {
    response = await client.send(
      new GetSecretValueCommand({
        SecretId: "YELP_API_KEY",
        VersionStage: "AWSCURRENT",
      }),
    );
  } catch (error) {
    throw error;
  }

  const secret = response.SecretString;

  return JSON.parse(secret).YELP_API_KEY;
};

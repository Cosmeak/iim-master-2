/* Amplify Params - DO NOT EDIT
	ENV
	REGION
	STORAGE_RESTAURANTS_ARN
	STORAGE_RESTAURANTS_NAME
	STORAGE_RESTAURANTS_STREAMARN
Amplify Params - DO NOT EDIT */ /* Amplify Params - DO NOT EDIT
    ENV
    REGION
    YELP_CLIENT_ID
    YELP_API_KEY
Amplify Params - DO NOT EDIT */

import AWS from "aws-sdk";
import crypto from "crypto";
import * as cheerio from "cheerio";
import {
  SecretsManagerClient,
  GetSecretValueCommand,
} from "@aws-sdk/client-secrets-manager";

const YELP_BASE_API_URL = "https://api.yelp.com/v3";
const dynamodb = new AWS.DynamoDB.DocumentClient();

/**
 * @type {import('@types/aws-lambda').APIGatewayProxyHandler}
 */
export const handler = async () => {
  let locations = ["Paris", "Lyon"];

  const promises = locations.map(async (location) =>
    getRestaurantsFromYelp(location),
  );

  const rawRestaurants = await Promise.all(promises).then((results) => {
    return results.flat();
  });

  let restaurants = [];
  for (const rawRestaurant of rawRestaurants) {
    const restaurant = await saveRestaurant(rawRestaurant);
    restaurants.push(restaurant);
  }
  // await getRestaurantComments(restaurants[0]);
  for (const restaurant of restaurants) {
    await getRestaurantComments(restaurant);
  }
};

const getRestaurantsFromYelp = async (location) => {
  const params = new URLSearchParams({
    term: "restaurants",
    location: location,
    limit: 10,
  }).toString();
  const url = `${YELP_BASE_API_URL}/businesses/search?${params}`;
  const response = await fetch(url, {
    headers: { Authorization: `BEARER ${await getYelpApiKey()}` },
  });
  const data = await response.json();

  return data.businesses;
};

const saveRestaurant = async (restaurant) => {
  const params = {
    TableName: process.env.STORAGE_RESTAURANTS_NAME,
    Item: {
      id: crypto.randomUUID(),
      yelpId: restaurant.id,
      name: restaurant.name,
      address: restaurant.location?.display_address,
      rating: restaurant.rating,
      url: restaurant.url.split("?")[0],
    },
  };

  await dynamodb.put(params).promise();

  return params.Item;
};

const getRestaurantComments = async (restaurant) => {
  let data = await (await fetch(restaurant.url)).text();
  const $ = cheerio.load(data);
  let comments = [];
  const rawComments = $("p.comment__09f24__D0cxf");

  for (const rawComment of rawComments) {
    comments.push($(rawComment).text());
  }

  const params = {
    TableName: "Restaurants-dev",
    Item: {
      comments: comments,
      ...restaurant,
    },
  };
  await dynamodb.put(params).promise();
};

const getYelpApiKey = async () => {
  // Use this code snippet in your app.
  // If you need more information about configurations or implementing the sample code, visit the AWS docs:
  // https://docs.aws.amazon.com/sdk-for-javascript/v3/developer-guide/getting-started.html
  const client = new SecretsManagerClient({
    region: "eu-west-3",
  });

  let response;

  try {
    response = await client.send(
      new GetSecretValueCommand({
        SecretId: "YELP_API_KEY",
        VersionStage: "AWSCURRENT", // VersionStage defaults to AWSCURRENT if unspecified
      }),
    );
  } catch (error) {
    // For a list of exceptions thrown, see
    // https://docs.aws.amazon.com/secretsmanager/latest/apireference/API_GetSecretValue.html
    throw error;
  }

  const secret = response.SecretString;

  return JSON.parse(secret).YELP_API_KEY;
};

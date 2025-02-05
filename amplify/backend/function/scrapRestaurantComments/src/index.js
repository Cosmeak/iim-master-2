/* Amplify Params - DO NOT EDIT
	ENV
	REGION
	STORAGE_REVIEWS_ARN
	STORAGE_REVIEWS_NAME
	STORAGE_REVIEWS_STREAMARN
Amplify Params - DO NOT EDIT */

import crypto from "crypto";
import * as cheerio from "cheerio";
import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import { PutCommand, DynamoDBDocumentClient } from "@aws-sdk/lib-dynamodb";

export const handler = async ({ id, url }) => {
  let data = await (await fetch(url)).text();
  const $ = cheerio.load(data);

  const rawComments = $("p.comment__09f24__D0cxf");
  for (const rawComment of rawComments) {
    const comment = $(rawComment).text();

    await saveComment(comment);
  }
};

const saveComment = async (text, restaurantId) => {
  const client = new DynamoDBClient({});
  const dynamodb = DynamoDBDocumentClient.from(client);

  const command = PutCommand({
    Table: process.env.STORAGE_REVIEWS_NAME,
    Item: {
      id: crypto.randomUUID(),
      restaurandId: restaurantId,
      feeling: null,
      text: text,
    },
  });

  await dynamodb.send(command);
};

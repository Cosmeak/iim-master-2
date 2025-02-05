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
import { LambdaClient, InvokeCommand } from "@aws-sdk/client-lambda";

export const handler = async ({ id, url }) => {
  let data = await (await fetch(url)).text();
  const $ = cheerio.load(data);

  const rawComments = $("p.comment__09f24__D0cxf");
  for (const rawComment of rawComments) {
    const textComment = $(rawComment).text();

    saveComment(textComment);
  }
};

const saveComment = async (text, restaurantId) => {
  const client = new DynamoDBClient({});
  const dynamodb = DynamoDBDocumentClient.from(client);

  const item = {
    id: crypto.randomUUID(),
    restaurandId: restaurantId,
    feeling: await getFeeling(text),
    text: text,
  };

  const command = PutCommand({
    Table: process.env.STORAGE_REVIEWS_NAME,
    Item: item,
  });

  const response = await dynamodb.send(command);
  if (response.$metadata.httpStatusCode == 200) return item;
};

const getFeeling = async (text) => {
  const lambdaClient = new LambdaClient({ region: "eu-west-3" });

  const command = new InvokeCommand({
    FunctionName: "ServerlessHuggingFaceStack-sentiment2141F307-wZgdaLJRxTCD",
    InvocationType: "RequestResponse",
    Payload: JSON.stringify({
      text: text,
    }),
  });

  const result = await lambdaClient.send(command);
  const asciiDecoder = new TextDecoder("ascii");
  const data = JSON.parse(asciiDecoder.decode(result.Payload));
  return data;
};

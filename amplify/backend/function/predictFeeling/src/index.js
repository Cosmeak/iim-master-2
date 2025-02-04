import {LambdaClient, InvokeCommand} from '@aws-sdk/client-lambda';

/**
 * @type {import('@types/aws-lambda').APIGatewayProxyHandler}
 */

const lambdaClient = new LambdaClient({ region: 'eu-west-3' });

export const handler = async (event) => {

  for (const restaurant of event) {

    for (const comment of restaurant.comments) {
      const command = new InvokeCommand({
        FunctionName: 'ServerlessHuggingFaceStack-sentiment2141F307-wZgdaLJRxTCD',
        InvocationType: 'RequestResponse',
        Payload: JSON.stringify({
          text: comment.comment,
        })
      });


      const result = await lambdaClient.send(command);

      const asciiDecoder = new TextDecoder('ascii');
      const data = JSON.parse(asciiDecoder.decode(result.Payload));

      console.log(data.body.label);

    }
  }

  return Promise.resolve('Successfully save feelings to DynamoDB');
}
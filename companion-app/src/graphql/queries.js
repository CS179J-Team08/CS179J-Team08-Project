/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const getAudio = /* GraphQL */ `
  query GetAudio($id: ID!) {
    getAudio(id: $id) {
      id
      name
      description
    }
  }
`;
export const listAudios = /* GraphQL */ `
  query ListAudios(
    $filter: ModelAudioFilterInput
    $limit: Int
    $nextToken: String
  ) {
    listAudios(filter: $filter, limit: $limit, nextToken: $nextToken) {
      items {
        id
        name
        description
      }
      nextToken
    }
  }
`;

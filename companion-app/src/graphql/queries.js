/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const getAudio = /* GraphQL */ `
  query GetAudio($id: ID!) {
    getAudio(id: $id) {
      id
      name
      owner
      file {
        bucket
        region
        key
      }
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
        owner
        file {
          bucket
          region
          key
        }
      }
      nextToken
    }
  }
`;
export const getFriend = /* GraphQL */ `
  query GetFriend($id: ID!) {
    getFriend(id: $id) {
      id
      username
      owner
    }
  }
`;
export const listFriends = /* GraphQL */ `
  query ListFriends(
    $filter: ModelFriendFilterInput
    $limit: Int
    $nextToken: String
  ) {
    listFriends(filter: $filter, limit: $limit, nextToken: $nextToken) {
      items {
        id
        username
        owner
      }
      nextToken
    }
  }
`;

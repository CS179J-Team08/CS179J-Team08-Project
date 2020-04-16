/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const getDump = /* GraphQL */ `
  query GetDump($id: ID!) {
    getDump(id: $id) {
      id
      name
      owner
      files {
        items {
          id
          bucket
        }
        nextToken
      }
    }
  }
`;
export const listDumps = /* GraphQL */ `
  query ListDumps(
    $filter: ModelDumpFilterInput
    $limit: Int
    $nextToken: String
  ) {
    listDumps(filter: $filter, limit: $limit, nextToken: $nextToken) {
      items {
        id
        name
        owner
        files {
          nextToken
        }
      }
      nextToken
    }
  }
`;
export const getAudio = /* GraphQL */ `
  query GetAudio($id: ID!) {
    getAudio(id: $id) {
      id
      dump {
        id
        name
        owner
        files {
          nextToken
        }
      }
      bucket
      fullsize {
        key
        size
        samples
      }
      length {
        key
        size
        samples
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
        dump {
          id
          name
          owner
        }
        bucket
        fullsize {
          key
          size
          samples
        }
        length {
          key
          size
          samples
        }
      }
      nextToken
    }
  }
`;

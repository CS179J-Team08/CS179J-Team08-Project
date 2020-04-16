/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const createDump = /* GraphQL */ `
  mutation CreateDump(
    $input: CreateDumpInput!
    $condition: ModelDumpConditionInput
  ) {
    createDump(input: $input, condition: $condition) {
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
export const updateDump = /* GraphQL */ `
  mutation UpdateDump(
    $input: UpdateDumpInput!
    $condition: ModelDumpConditionInput
  ) {
    updateDump(input: $input, condition: $condition) {
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
export const deleteDump = /* GraphQL */ `
  mutation DeleteDump(
    $input: DeleteDumpInput!
    $condition: ModelDumpConditionInput
  ) {
    deleteDump(input: $input, condition: $condition) {
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
export const createAudio = /* GraphQL */ `
  mutation CreateAudio(
    $input: CreateAudioInput!
    $condition: ModelAudioConditionInput
  ) {
    createAudio(input: $input, condition: $condition) {
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
export const updateAudio = /* GraphQL */ `
  mutation UpdateAudio(
    $input: UpdateAudioInput!
    $condition: ModelAudioConditionInput
  ) {
    updateAudio(input: $input, condition: $condition) {
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
export const deleteAudio = /* GraphQL */ `
  mutation DeleteAudio(
    $input: DeleteAudioInput!
    $condition: ModelAudioConditionInput
  ) {
    deleteAudio(input: $input, condition: $condition) {
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

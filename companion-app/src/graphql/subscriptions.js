/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const onCreateDump = /* GraphQL */ `
  subscription OnCreateDump {
    onCreateDump {
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
export const onUpdateDump = /* GraphQL */ `
  subscription OnUpdateDump {
    onUpdateDump {
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
export const onDeleteDump = /* GraphQL */ `
  subscription OnDeleteDump {
    onDeleteDump {
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
export const onCreateAudio = /* GraphQL */ `
  subscription OnCreateAudio {
    onCreateAudio {
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
export const onUpdateAudio = /* GraphQL */ `
  subscription OnUpdateAudio {
    onUpdateAudio {
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
export const onDeleteAudio = /* GraphQL */ `
  subscription OnDeleteAudio {
    onDeleteAudio {
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

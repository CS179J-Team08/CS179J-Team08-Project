/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const onCreateAudio = /* GraphQL */ `
  subscription OnCreateAudio($owner: String!) {
    onCreateAudio(owner: $owner) {
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
export const onUpdateAudio = /* GraphQL */ `
  subscription OnUpdateAudio($owner: String!) {
    onUpdateAudio(owner: $owner) {
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
export const onDeleteAudio = /* GraphQL */ `
  subscription OnDeleteAudio($owner: String!) {
    onDeleteAudio(owner: $owner) {
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
export const onCreateFriend = /* GraphQL */ `
  subscription OnCreateFriend {
    onCreateFriend {
      id
      username
      owner
    }
  }
`;
export const onUpdateFriend = /* GraphQL */ `
  subscription OnUpdateFriend {
    onUpdateFriend {
      id
      username
      owner
    }
  }
`;
export const onDeleteFriend = /* GraphQL */ `
  subscription OnDeleteFriend {
    onDeleteFriend {
      id
      username
      owner
    }
  }
`;

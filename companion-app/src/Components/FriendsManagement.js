import React, { Component } from 'react';
import * as queries from '../graphql/queries';
import * as mutations from '../graphql/mutations';
import { Auth, API, graphqlOperation } from 'aws-amplify';

export default class FriendsManagement extends Component {
    constructor(props) {
        super(props);

        this.state = {
            showingFriendsList: false,
            friendToAdd: "",
            userFriends: []
        }
    }

    updateFriendToAdd = (event) => {
        this.setState({
            friendToAdd: event.target.value
        });
    }

    addToFriendsList = async () => {
        let user = await Auth.currentAuthenticatedUser();

        const { attributes } = user;

        const friendDetails = {
            username: this.state.friendToAdd,
            owner: attributes.sub
        };

        const newFriend = await API.graphql(graphqlOperation(mutations.createFriend, {input: friendDetails}));
        console.log(newFriend);

        this.setState({
            friendToAdd: ""
        });
    }

    showHideFriends = async () => {
        let userFriends = [];

        if(!this.state.showingFriendsList) {
            // this is all friends in the database regardless of current user
            const allFriends = await API.graphql(graphqlOperation(queries.listFriends));
            const friendArray = allFriends.data.listFriends.items;

            let user = await Auth.currentAuthenticatedUser();
            const { attributes } = user;

            for (const i in friendArray) {
                if(friendArray[i].owner === attributes.sub) {
                    console.log(friendArray[i].username);
                    userFriends.push(friendArray[i].username);
                }
            }
        }

        let friendListState = !this.state.showingFriendsList;
        this.setState({ showingFriendsList: friendListState, userFriends: userFriends });
    }

    render() {
        return (
            <>
                <div>
                    <p></p>
                    <input type="text" onChange={this.updateFriendToAdd} />
                    <button onClick={this.addToFriendsList}>Add Friend</button>
                    <button onClick={this.showHideFriends}>Show/Hide Friends</button>
                    <ul>
                        { this.state.userFriends.map(listFriends => (
                            <li> { listFriends } </li>
                        )) }
                    </ul>
                </div>
            </>
        );
    }
}
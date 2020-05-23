import React, { Component } from 'react';

export default class FriendsManagement extends Component {
    constructor(props) {
        super(props);

        this.state = {
            showingFriendsList: false,
            friendToAdd: ""
        }
    }

    updateFriendToAdd = (event) => {
        this.setState({
            friendToAdd: event.target.value
        });
    }

    addToFriendsList = () => {
        console.log(this.state.friendToAdd);

        this.setState({
            friendToAdd: ""
        });
    }

    render() {
        return (
            <>
                <div>
                    <p></p>
                    <input type="text" onChange={this.updateFriendToAdd} />
                    <button onClick={this.addToFriendsList}>Add Friend</button>
                </div>
            </>
        );
    }
}
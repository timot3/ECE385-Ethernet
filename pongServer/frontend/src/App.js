import React from 'react';
import Button from '@material-ui/core/Button';
import arrow from "./arrow.svg"

const sendUrl = "/sendPress";
// const sendUrl = "http://alma.lol:120/sendPress";

class App extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      player: 1,
      direction: 0,
      playerTxt: "Player 1"
    }

    this.uparrowStyle = {
      width:251,
      height:300,
      background: `url(${arrow})`,
      transform: "rotate(270deg)",
      backgroundRepeat: "no-repeat",
      backgroundSize: "auto",
      backgroundPosition: "center",
    };

    this.downarrowStyle = {
      width:251,
      height:300,
      background: `url(${arrow})`,
      transform: "rotate(90deg)",
      backgroundRepeat: "no-repeat",
      backgroundSize: "auto",
      backgroundPosition: "center",
    };

    this.noSelect = {
      MozUserSelect: "none",
      WebkitUserSelect: "none",
      msUserSelect: "none",
    }
  }

  async onMouseDown(direction) {
    this.setState({ direction });

    var body = {
      "player": this.state.player,
      "direction": direction
    }

    const response = await fetch(sendUrl, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(body)
    });

    await response;
  }

  async onMouseUp() {
    var body = {
      "player": this.state.player,
      "direction": 0
    }

    const response = await fetch(sendUrl, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(body)
    });

    await response;
  }

  render() {
    return(
      <div className="zoomControl" style={this.noSelect}>
        <div className="zoom"></div><center>

        <Button className="zoomIn" style={this.uparrowStyle} onMouseUp={() => this.onMouseUp()} onMouseDown={() => this.onMouseDown(1)}></Button>
        <br></br>
        <Button className="zoomIn"style={this.downarrowStyle} onMouseUp={() => this.onMouseUp()} onMouseDown={() => this.onMouseDown(2)}></Button>

        <br></br><br></br>
        
        <Button variant="contained" onClick={async () => {
          this.setState({ player: 1 });
          this.setState({ playerTxt: "Player 1"});
        }}>Set Player 1</Button>

        <Button variant="contained" onClick={async () => {
          this.setState({ player: 2 });
          this.setState({ playerTxt: "Player 2"});
        }}>Set Player 2</Button>
        <br></br>
        <p>Currently {this.state.playerTxt}</p>

        <br></br><br></br><br></br><br></br><br></br><br></br>
        <small>ECE 385 FA21 Final Project - Ethernet of DE10-Lite</small><br></br>
        <small>Hassan Farooq, Tim Vitkin</small>
      </center></div>
    )
  }
}

export default App;
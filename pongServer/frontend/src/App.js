import React from 'react';
import Button from 'react-bootstrap/Button';

const sendUrl = "http://192.168.0.124:3080/sendPress";

class Zoom extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      player: 1,
      direction: 0,
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
    return <div className="zoomControl" >
      <div className="zoom"></div>

      <Button variant="contained" onClick={async () => {
        this.setState({ player: 1 });
      }}>set p1</Button>

      <Button variant="contained" onClick={async () => {
        this.setState({ player: 2 });
      }}>set p2</Button>

      <button className="zoomIn" onMouseUp={() => this.onMouseUp()} onMouseDown={() => this.onMouseDown(1)}>up</button>
      <button className="zoomIn" onMouseUp={() => this.onMouseUp()} onMouseDown={() => this.onMouseDown(2)}>down</button>
    </div>
  }
}

export default Zoom;
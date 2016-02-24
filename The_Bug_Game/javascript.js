/**
 * Created by shreymahendru on 2016-02-08.
 */



var canvas;
var ctx;
var infoBar;
var ctxInfoBar;
var foods = []; //array that has all the foods
var bugs = [];
var interval_game;
var game_pause = false;
var frameRate= 1000/60;
var overlapDist = 5;
var interval_bugs;
var fadeList = [];
var scores = [0, 0];
var timer = 60;
var timeCntr =0;
var level = 1;


window.onload =function()
{
    document.getElementById("startMenu").style.display = "block";
    document.getElementById("game").style.display = "none";
    
    var hs = [localStorage.getItem('hs1'), localStorage.getItem('hs2')];
    document.getElementById("highScore").innerHTML = "High Score: <br>" + hs[level - 1];
    document.getElementById("startButton").onclick = startGame;
    document.getElementById("radio1").onchange = setHighScore;
    document.getElementById("radio2").onchange = setHighScore;
    
    
    canvas = document.getElementById("viewport");
    ctx =canvas.getContext("2d");
    infoBar = document.getElementById("infoBar");
    ctxInfoBar = infoBar.getContext("2d");
    document.getElementById("viewport").onclick = killBug;
    document.getElementById("infoBar").onclick = checkPause; // check pause on canvas and execute
};

function startGame(){
    console.log("game started!");
    document.getElementById("game").style.display = "block";
    document.getElementById("startMenu").style.display = "none";
    
    bugs = [];
    foods = [];
    scores[level-1] = 0;
    timer = 60;
    
    drawInfoBar();
    foods = createGame();
    interval_game =  window.setInterval(reDraw, frameRate);
    interval_bugs = window.setInterval(createBug, (Math.random() * 2 + 1)*1000); //new bugs created at randome times
}

function showMenu(){
    document.getElementById("game").style.display = "none";
    document.getElementById("startMenu").style.display = "block";
    setHighScore();
}

function setHighScore() {
    if (document.getElementById("radio1").checked) {
        level = 1;
    }
    else {
        level = 2;
    }
    var hs = [localStorage.getItem('hs1'), localStorage.getItem('hs2')];
    document.getElementById("highScore").innerHTML = "High Score: <br>" + hs[level - 1];
}


function checkPause(event)
{
    var x= event.pageX - infoBar.offsetLeft;
    var y= event.pageY - infoBar.offsetTop;
    if (x >=187 && x<=247 && y>= 10 && y<= 40)
    {
        pause();
    }
}


function pause()
{
    if(!game_pause)
    {
        game_pause = true;
        console.log("pause");
        window.clearInterval(interval_game);
        window.clearInterval(interval_bugs);
        
    }
    else if (game_pause)
    {
        game_pause = false;
        console.log("not pause");
        interval_game = window.setInterval(reDraw, frameRate);
        interval_bugs = window.setInterval(createBug, (Math.random() * 2 + 1)*1000);
        
    }
    ctxInfoBar.clearRect(0, 0, infoBar.width, infoBar.height);  // clear info bar and redraw
    drawInfoBar(); // toogle pause/paly UI
    
}

function drawInfoBar()    // to draw in the info bar canvas
{
    ctxInfoBar.font = "20px Comic Sans MS";
    ctxInfoBar.fillStyle = "#F9D956";
    ctxInfoBar.fillText("Time Left: "+ timer,6,30);
    ctxInfoBar.fillText("Score: "+ scores[level-1], 300, 30);
    ctxInfoBar.fillStyle = "purple";
    ctxInfoBar.fillRect(187, 10, 60, 30);
    ctxInfoBar.fillStyle = "#F9D956";
    ctxInfoBar.stroke();
    ctxInfoBar.font = "15px Comic Sans MS";
    if(!game_pause) {
        ctxInfoBar.fillText("Pause", 197, 30);
    }
    else
    {
        ctxInfoBar.fillText("Play", 202, 30);
    }
}


function reDraw()
{
    timeCntr++;
    ctxInfoBar.clearRect(0, 0, infoBar.width, infoBar.height);  // clear info bar and redraw
    drawInfoBar();
    if(timeCntr ==60)
    {
        timer--;
        timeCntr = 0;
    }
    ctx.clearRect(0, 0, canvas.width, canvas.height); //  clear canvas and redraw everything again
    if(foods.length >0) {
        // draw fade list
        if(fadeList.length >0)
        {
            for (var j = 0; j < fadeList.length; j++)
            {
                if(fadeList[j]["alpha"]> 0) {
                    fadeList[j]["alpha"] -= .5 / 20;
                    if(fadeList[j]["alpha"]> 0) {
                        fadeList[j].drawBug(ctx);
                    }
                }
                else
                {
                    fadeList.splice(j, 1);
                }
            }
        }
        for (var i = 0; i < foods.length; i++) { //draw food
            foods[i].drawFood(ctx);
        }
        moveBugs();
    }
    else
    {
        alert("You lost");
        console.log("you lost");
        window.clearInterval(interval_bugs);
        window.clearInterval(interval_game);
        showMenu();
        
    }
    if(timer == 0)
    {
        ctxInfoBar.clearRect(0, 0, infoBar.width, infoBar.height);  // clear info bar and redraw
        drawInfoBar();
        window.clearInterval(interval_game);
        window.clearInterval(interval_bugs);
        timer = 0;
        
        if (level == 1){
            level = 2;
            var alertText = "Level 2" + "\n" + "Level 1 score: " + scores[0];
            alert(alertText);
            startGame();
            
        }else {
            var alertText = "You win!" + "\n" + "Level 1 score: " + scores[0] + "\n" + "Level 2 score: " + scores[1] ;
            alert(alertText);
            showMenu();
        }
    }
}

function moveBugs()
{
    if(bugs.length> 0) {
        for (var i = 0; i < bugs.length; i++) {
            if (foods.length > 0) {
                var closesFood = findClosestFood(bugs[i].x, bugs[i].y);
                var dx = closesFood[0]- bugs[i].x  ;
                var dy = closesFood[1] - bugs[i].y   ;
                var angle = Math.atan2(dy, dx);
                var velo = bugs[i].velocity;
                var xVelocity = velo *Math.cos(angle);   // this shit works
                var yVelocity = velo *Math.sin(angle);
                var change = checkBugsAround(bugs[i]);
                if (change == -1) {
                    bugs[i].y += yVelocity;
                    bugs[i].x += xVelocity;
                }
                else if (change == -5)
                {
                    bugs[i].x -= 5;
                }
                else if (change == 5)
                {
                    bugs[i].x  += 5;
                }
                //console.log(bugs[i].y);  // HAVE TO FIND THE NEAREST BUG AND MOVE LIKEWISE
                bugs[i].drawBug(ctx);
            }
        }
    }
}

function checkBugsAround(currentBug)  // returns -1 so the bug can move as usual and move the bug 5px away from the faster bug
// and other cases to avoid collision
{
    if (bugs.length > 1) {
        for (var i = 0; i < bugs.length; i++) {
            if(bugs[i] != currentBug)
            {
                var dist = Math.sqrt(Math.pow((bugs[i].x-currentBug.x),2)+ Math.pow((bugs[i].y-currentBug.y),2));
                if (dist <40)
                {
                    //same speed  right bug passes
                    if(bugs[i].color == currentBug.color)
                    {
                        if(currentBug.x < bugs[i].x) //current bug is the left one
                        {
                            // bugs[i].y -= 2;
                            return -1;
                        }
                        else
                        {
                            return -100;
                        }
                    }
                    if(currentBug.color == "orange") //slowest bug has to wait and move to the left/right
                    {
                        if(currentBug.x < bugs[i].x)
                        {
                            currentBug.x -= 5;
                            return -5;
                        }
                        else
                        {
                            currentBug.x += 5;
                            return 5;
                        }
                    }
                    if (currentBug.color == "red" && bugs[i].color == "black") //medium speed only wait and move for black
                    {
                        if(currentBug.x < bugs[i].x)
                        {
                            currentBug.x -= 5;
                            return -5;
                        }
                        else
                        {
                            currentBug.x += 5;
                            return 5;
                        }
                    }
                    if(currentBug.color == "black")
                    {
                        if(currentBug.x < bugs[i].x)
                        {
                            bugs[i].x += 5;
                            bugs[i].y -=2;
                            return -1;
                        }
                        else
                        {
                            bugs.x -= 5;
                            bugs[i].y -=2;
                            return -1;
                        }
                    }
                }
                
                
            }
        }
    }
    return -1;
}
function findClosestFood(x1, y1)
{
    var min= Number.MAX_VALUE;
    var coord =[];
    for (var i = 0; i< foods.length; i++)
    {
        x2 = foods[i].x;
        y2 = foods[i].y;
        
        var dist = Math.sqrt(Math.pow((x2-x1),2)+ Math.pow((y2-y1),2));
        if(dist < overlapDist)
        {
            foods.splice(i, 1);
            return findClosestFood(x1, x2);
        }
        
        if (dist < min)
        {
            min = dist;
            coord = [x2,y2];
        }
        
    }
    return coord;
}

function createBug()
{
    console.log("new bug");
    var x=Math.floor( (Math.random() * (390-10)+10)); //x at random between 10 and 390
    // var y = 20;  //start at 20px
    //getting random colors
    var color;
    var velocity;
    var probability = Math.random();
    if (probability < .3)
    {
        color = "black";
        if (level == 1) {
            velocity = 150 / 60;
        }
        else {
            velocity = 200/60;
        }
    }
    else if (probability >= .3 && probability <= .6)
    {
        color = "red";
        if (level == 1) {
            velocity = 75 / 60;
        }
        else {
            velocity = 100/60;
        }
    }
    else if (probability > .6)
    {
        color = "orange";
        if (level == 1) {
            velocity = 60 / 60;
        }
        else {
            velocity = 80/60;
        }
    }
    console.log(velocity);
    var bug = new Bug(x, 20, color, velocity, .7);
    bugs.push(bug);
    bug.drawBug();
    
}


var Food  = function(x, y, eaten) {  //food object has x and y coordinates and if it is eaten or not
    this.x =x;
    this.y= y;
    this.eaten= eaten;
    
};

var Bug  = function(x, y, color, velocity, alpha) {  //Bug object has x and y coordinates and color
    this.x =x;
    this.y =y;
    this.color= color;
    this.velocity = velocity;
    this.alpha = alpha;
    
};

function createGame()
{
    var allFood = [];
    for(var i = 0; i < 5; i++)
    {
        var crd = getFoodCoord();
        var food1 = new Food(crd[0], crd[1], false);
        allFood.push(food1);
        ctx.globalAlpha =1;
        food1.drawFood(ctx);
    }
    return allFood;
}


Bug.prototype.drawBug = function(ctx){
    makeBug(this.x, this.y, this.color, this.alpha);
};


Food.prototype.drawFood = function(ctx){    // checks if the food is eaten or not and draws an image
    var img = document.getElementById("food");
    if (!this.eaten) {
        ctx.globalAlpha=1;
        ctx.drawImage(img, this.x, this.y, 18, 18);
    }
};

function getFoodCoord()  // generates random x and y coordinates
{
    var miny =  .20* canvas.height;  // not in top  20%
    var maxy = canvas.height- 10 ;
    var y= Math.floor(Math.random() * (maxy - miny) + miny);
    var x=Math.floor( (Math.random() * (canvas.width-20)+10));
    return [x, y];
}


function killBug(event)
{
    if(!game_pause && bugs.length>0)
    {
        var x1= event.pageX - canvas.offsetLeft;
        var y1= event.pageY - canvas.offsetTop;
        for (var i = 0 ; i < bugs.length; i++)
        {
            x2 = bugs[i].x;
            y2 = bugs[i].y;
            var dist = Math.sqrt(Math.pow((x2-x1),2)+ Math.pow((y2-y1),2));
            if (dist <= 70 ) //30px radius
            {
                if(bugs[i].color == "black")
                {
                    scores[level-1] += 5;
                }
                else if (bugs[i].color == "red")
                {
                    scores[level-1] += 3;
                }
                else
                {
                    scores[level-1] += 1;
                }
                
                
                var hs = [localStorage.getItem('hs1'), localStorage.getItem('hs2')];
                
                if(scores[level-1] > hs[level - 1]){
                    var key = "hs" + level;
                    localStorage.setItem(key, scores[level-1]);
                    
                }
                fadeList.push(bugs[i]);
                bugs.splice(i, 1);
                
            }
        }
    }
}






function makeBug(x, y, color, alpha) {
    
    color = color;
    //alpha = ".5";
    
    //http://www.w3schools.com/tags/canvas_globalalpha.asp
    ctx.globalAlpha = alpha;
    
    /*-- Whiskers, legs and arms--*/
    ctx.beginPath();
    ctx.moveTo(x, y);
    ctx.lineTo(x + 5, y - 15);
    ctx.lineTo(x + 10, y);
    ctx.moveTo(x + 5, y - 20);
    ctx.lineTo(x + 4, y - 22);
    ctx.lineTo(x + 6, y - 22);
    ctx.lineTo(x + 5, y - 20);
    ctx.moveTo(x, y - 20);
    ctx.lineTo(x + 10, y - 40);
    ctx.moveTo(x + 10, y - 20);
    ctx.lineTo(x, y - 40);
    ctx.lineWidth = 2;
    ctx.strokeStyle = color;
    
    /*-- Triangles on the tips --*/
    ctx.moveTo(x, y);
    ctx.lineTo(x, y - 3);
    ctx.lineTo(x + 1.73, y - 2.4);
    ctx.lineTo(x, y);
    ctx.moveTo(x + 10, y);
    ctx.lineTo(x + 8.27, y - 2.4);
    ctx.lineTo(x + 10, y - 3);
    ctx.lineTo(x + 10, y);
    ctx.moveTo(x, y - 20);
    ctx.lineTo(x, y - 22);
    ctx.lineTo(x + 1.6, y - 21.25);
    ctx.lineTo(x, y - 22);
    ctx.moveTo(x + 10, y - 20);
    ctx.lineTo(x + 8.4, y - 21.25);
    ctx.lineTo(x + 10, y - 22);
    ctx.lineTo(x + 10, y - 20);
    ctx.moveTo(x, y - 40);
    ctx.lineTo(x, y - 38);
    ctx.lineTo(x + 1.6, y - 38.25);
    ctx.lineTo(x, y - 38);
    ctx.moveTo(x + 10, y - 40);
    ctx.lineTo(x + 8.4, y - 38.25);
    ctx.lineTo(x + 10, y - 38);
    ctx.lineTo(x + 10, y - 40);
    ctx.stroke();
    
    /*-- Body parts --*/
    ctx.beginPath();
    ctx.arc(x + 5, y - 15, 5, 0, 2 * Math.PI);
    ctx.moveTo(x + 5, y - 21);
    ctx.bezierCurveTo(x, y - 20, x, y - 30, x + 5, y - 38.75);
    ctx.moveTo(x + 5, y - 21);
    ctx.bezierCurveTo(x + 10, y - 20, x + 10, y - 30, x + 5, y - 38.75);
    ctx.fillStyle = color;
    ctx.lineWidth = 1;
    ctx.strokeStyle = "#000000"
    ctx.stroke();
    ctx.fill();
    
    /*-- Eyes and Mouth --*/
    ctx.beginPath();
    ctx.arc(x + 3.3, y - 13.2, 1, 0, 2 * Math.PI);
    ctx.arc(x + 6.75, y - 13.2, 1, 0, 2 * Math.PI);
    ctx.fillStyle = "white";
    ctx.fill();
    ctx.beginPath();
    ctx.arc(x + 5, y - 15, 2.5, 0, Math.PI, false);
    ctx.stroke();
}


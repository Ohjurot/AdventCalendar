/*
 *  Custom script for rendering the advent index page
 */

let AdventHover = null;
let AdventLinks = null;

$( document ).ready(function() 
{
    adventInvokeDrawCanvas();
});

$(window).on('resize', function()
{
    adventInvokeDrawCanvas();
});

$("#canvas").mousemove(function( event ) {
    const canvasParentPadding = parseInt($("#mainContainer").css('padding-left'))
    const parentOffset = $(this).parent().offset();
    const x = (event.pageX - parentOffset.left - canvasParentPadding); 
    const y = (event.pageY - parentOffset.top - canvasParentPadding);

    found = false;
    for(i = 0; i < AdventLinks.length; i++)
    {
        // Extract args
        params = new Array();
        params = AdventLinks[i].split(";");
        l_x = parseInt(params[0]);
        l_y = parseInt(params[1]);
        l_sx = parseInt(params[2]);
        l_sy = parseInt(params[3]);
        l_url = params[4];

        // Check if valid
        if(
            x >= l_x && x <= l_x + l_sx &&
            y >= l_y && y <= l_y + l_sy
        )
        {
            document.body.style.cursor = "pointer";
            AdventHover = l_url;
            found = true;
            break;
        }
    }

    if(!found)
    {
        AdventHover = null;
        document.body.style.cursor = "";
    }
})

$("#canvas").click(function( event ) {
    if(AdventHover)
    {
        window.location = AdventHover;
    }
})

function adventInvokeDrawCanvas()
{
    jsonFilePath = $("#jsonFilePath").attr("href");

    // Invoke request
    xmlhttp = new XMLHttpRequest();
    xmlhttp.open("GET", jsonFilePath, true);
    xmlhttp.onload = (e) => 
        {
            if (xmlhttp.readyState == 4 && xmlhttp.status == 200) 
            {
                adventDrawCanvas(JSON.parse(xmlhttp.responseText));
            }
        };
    xmlhttp.onerror = (e) => 
    {
        console.error(xmlhttp.statusText);
    };
    xmlhttp.send(null);
}

function adventDrawCanvas(positionData)
{
    const canvas = document.getElementById('canvas');
    const canvasParent = document.getElementById('mainContainer');
    const canvasCtx = canvas.getContext('2d');

    // Draw image
    adventSetCanvasSize(canvas, canvasParent, 0.668)
    canvasCtx.drawImage(
        document.getElementById("canvasImage"), 
        0, 0,
        canvas.width, canvas.height
    );

    // Easy access json
    display = positionData.display;
    positions = positionData.positions;

    // Compute gernal metrics
    const margin = canvas.width * 0.016;
    const fontRatio = 5;
    const sx = 
        (canvas.width - (display.columns + 1) * margin) / display.columns;
    const sy = 
        (canvas.height - (display.rows + 1) * margin) / display.rows;

    // Font size
    const fontSize = Math.min(sx, sy) - (margin / fontRatio) * 2;

    // Link array
    AdventHover = null;
    AdventLinks = new Array();

    // For each day
    for(i = 0; i < 24; i++)
    {
        // Draw
        x = positions["" + (i+1)].x
        y = positions["" + (i+1)].y
        rx = margin + (margin + sx) * x;
        ry = margin + (margin + sy) * y;
        adventDrawDay(canvasCtx, "" + (i + 1), "orange", fontSize,
            rx, ry, 
            sx, sy
        );

        // Place hyperlink
        if($("#day" + (i + 1)).length != 0)
        {
            linkInfo = rx + ";" + ry + ";" + sx + ";" + sy + ";" + "/day/" + (i + 1);
            AdventLinks.push(linkInfo);
        }
    }
}

function adventDrawDay(ctx, day, color, fsize, dx, dy, sx, sy)
{
    // Draw rect
    ctx.strokeStyle = color;
    ctx.strokeRect(dx, dy, sx, sy);
    ctx.fillStyle = "rgba(0.1, 0.1, 0.1, 0.2)";
    ctx.fillRect(dx, dy, sx, sy);

    // Draw text
    ctx.font = fsize + "px Arial";
    ctx.fillStyle = color;
    dim = ctx.measureText(day);
    ctx.fillText(
        day,
        dx + (sx - dim.width) / 2,
        dy + (sy - fsize) / 2 + (fsize - fsize * 0.1)
    );
}

function adventSetCanvasSize(canvas, canvasParent, imageAspectRatio)
{
    canvasParentPadding = parseInt($(canvasParent).css('padding-left'))
    canvas.width = canvasParent.clientWidth - canvasParentPadding * 2;
    canvas.height = canvas.width * imageAspectRatio;
}

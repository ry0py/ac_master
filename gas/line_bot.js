// ↓↓↓↓↓ 自分で書き換えるところ ↓↓↓↓↓　=========================================================================================================
// LINE Bot 設定
const CHANNEL_ACCESS_TOKEN = 'xxxxxxxx'; 


// その他設定
const logSheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName('log');
var replyToken, json


//ポストで送られてくるので、ポストデータ取得
function doPost(e) {
  // 動作確認用のログ出力
  // LINEBotから送られてきたデータを、プログラムで利用しやすいようにJSON形式に変換する
  json = JSON.parse(e.postData.contents);

  //返信するためのトークン取得
  log_to_sheet("B",json);
  replyToken= json.events[0].replyToken;
  if (typeof replyToken === 'undefined') {
    return;
  }

  // 返信するメッセージを作成
  messages = message_send();

  // loading(json.events[0].source.userId)

  // line-bot-sdk-gas のライブラリを利用しています ( https://github.com/kobanyan/line-bot-sdk-gas )
  const linebotClient = new LineBotSDK.Client({ channelAccessToken: CHANNEL_ACCESS_TOKEN });

  // メッセージを返信
  try{
    linebotClient.replyMessage(replyToken, messages);
  }catch(e){
    // うまく動作しない場合は、エラーが発生していないか確認してみましょう
    // log_to_sheet("A", e)
  }

  return ContentService.createTextOutput(JSON.stringify({'content': 'post ok'})).setMimeType(ContentService.MimeType.JSON);
}

// Lineのデータをもとにbeeboteにpubする内容を決定
function message_send() {
  //送られたLINEメッセージを取得
  var user_message = json.events[0].message.text;
  var reply_messages = []; 
  var suc_pub = false;

  if(user_message === "on"){
    reply_messages.push("エアコンをオンにします");
    suc_pub = beebottePub("ac_on");
  } else if(user_message === "off"){
    reply_messages.push("エアコンをオフにします");
    suc_pub = beebottePub("ac_off");
  }
  else{
    reply_messages = ["そのような操作は存在しません。\n赤ちゃんからやり直してください"];
  }
  reply_messages.push(suc_pub ? "送信に成功しました":"送信に失敗しました")
  

  // メッセージを整形
  var messages = reply_messages.map(function (v) {
    return {'type': 'text', 'text': v};    
  });

  return messages
}

// beeboteに送るための関数
function beebottePub(action) {
  var headers = {
    "Content-Type": "application/json",
    "X-Auth-Token": "token_xxxxxxxxx"
  };
  var json = `{"data":"${action}"}`;
  var options = {
    "headers": headers,
    "method": "post",
    "payload": json
  };
  
  // GASの組み込み関数HTTPリクエストを送る
  try{
    UrlFetchApp.fetch("https://api.beebotte.com/v1/data/publish/AirMaster/AirMaster", options); // api参照(https://beebotte.com/api/play),data参照(https://beebotte.com/docs/publish#:~:text=Server%20side%20Publish%20(REST%20API))
    return true;
  }
  catch(error){
    return false;
  }
}

// 処理の確認用にログを出力する関数
function log_to_sheet(column, text) {
  if(logSheet.getRange(column + "1").getValue() == ""){
    lastRow = 0
  } else if(logSheet.getRange(column + "2").getValue() == ""){
    lastRow = 1
  } else {
    var lastRow = logSheet.getRange(column + "1").getNextDataCell(SpreadsheetApp.Direction.DOWN).getRow();
    // 無限に増えるので1000以上書き込んだらリセット
    console.log("lastRow", lastRow)
    if(lastRow >= 1000){
      logSheet.getRange(column + "1:" + column + "10").clearContent()
      lastRow = 0
    }
  }
  var putRange = column + String(lastRow + 1)
  logSheet.getRange(putRange).setValue(text);
}


// ローディング機能
// 1対1のチャットボットにしか使えないので、グループなど複数人が参加できるBotでは使えないみたいです。
// 詳細はこちら https://developers.line.biz/ja/reference/messaging-api/#display-a-loading-indicator
function loading(userId) {
  UrlFetchApp.fetch('https://api.line.me/v2/bot/chat/loading/start', {
    'headers': {
      'Content-Type': 'application/json; charset=UTF-8',
      'Authorization': 'Bearer ' + CHANNEL_ACCESS_TOKEN,
    },
    'method': 'post',
    'payload': JSON.stringify({
      'chatId': userId,
      'loadingSeconds': 20
    }),
  });
}
package com.titansdk.titansdkunittest;

import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.widget.TextView;
import android.util.Log;

public class MainActivity extends AppCompatActivity {

    private TextView utResult;
    private String utCase;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Bundle extras = this.getIntent().getExtras();
        if ((extras == null) ||
	    !extras.containsKey("TT_CASE") ||
	    ((utCase = extras.getString ("TT_CASE")) == null)) {
            Log.d("platform", "no TT_CASE set");
            return;
        }
        Log.d("platform", "TT_CASE: " + utCase);

        utResult = (TextView) findViewById(R.id.ut_text);
        utResult.setMovementMethod(new ScrollingMovementMethod());

        new UTTask().execute();
    }

    private class UTTask extends AsyncTask<Void, Void, String> {

        @Override
        protected String doInBackground(Void... voids) {
            return TTUnitTestJNI.runUT(utCase);
        }

        @Override
        protected void onPostExecute(String s) {
            utResult.setText(s);
        }
    }
}

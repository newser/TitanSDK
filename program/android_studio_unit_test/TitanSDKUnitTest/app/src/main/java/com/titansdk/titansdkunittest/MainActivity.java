package com.titansdk.titansdkunittest;

import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private TextView utResult;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        utResult = (TextView) findViewById(R.id.ut_text);
        utResult.setMovementMethod(new ScrollingMovementMethod());

        Button btn = (Button)findViewById(R.id.ut_run);
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                utResult.setText("Running unit test, please wait...");
                view.setEnabled(false);
                new UTTask().execute();
            }
        });
    }

    private class UTTask extends AsyncTask<Void, Void, String> {

        @Override
        protected String doInBackground(Void... voids) {
            return TTUnitTestJNI.runUT("");
        }

        @Override
        protected void onPostExecute(String s) {
            utResult.setText(s);
        }
    }
}

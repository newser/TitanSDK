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

        String tt_case = getIntent().getStringExtra("TT_CASE");
        if (tt_case != null) {
            String[] name = { tt_case };
            new UTTask().execute(name);
        } else {
            final String[] name = { "case_rbtree" };
            Button btn = (Button)findViewById(R.id.ut_run);
            btn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    utResult.setText("Running unit test, please wait...");
                    view.setEnabled(false);
                    new UTTask().execute(name);
                }
            });
        }
    }

    private class UTTask extends AsyncTask<String, Void, String> {

        @Override
        protected String doInBackground(String... name) {
            String r = "";
            for (int i = 0; i < name.length; ++i) {
                r += TTUnitTestJNI.runUT(name[i]);
            }
            return r;
        }

        @Override
        protected void onPostExecute(String s) {
            utResult.setText(s);
        }
    }
}
